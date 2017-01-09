// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module http of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
//
// Original sources:
//   https://github.com/shatilov-diman/pisk/
//   https://bitbucket.org/charivariltd/pisk/
//
// Author contacts:
//   Dmitry Shatilov (e-mail: shatilov.diman@gmail.com; site: https://www.linkedin.com/in/shatilov)
//
//


#pragma once

#include <pisk/infrastructure/Logger.h>
#include <pisk/utils/algorithm_utils.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <curl/curl.h>
#undef min

#include <cstring>
#include <memory>
#include <deque>
#include <map>

#include "Worker.h"
#include "CurlCABundle.h"

namespace pisk
{
namespace services
{
namespace http
{
	class cURLWorker :
		public Worker
	{
		static constexpr const std::size_t task_limit = 10;

		struct CurlTask
		{
			HttpTaskPtr http_task;
			curl_slist* headers = nullptr;
			std::size_t posted_data_size = 0;
		};
		using CurlTaskPtr = std::unique_ptr<CurlTask>;

		std::map<CURL*, CurlTaskPtr> tasks;
		std::deque<HttpTaskPtr> completed_tasks;

		CURLM* multi_handle = nullptr;

	private:
		virtual void init_service() noexcept final override
		{
			infrastructure::Logger::get().info("http", "Curl: init curl");

			curl_global_init(CURL_GLOBAL_DEFAULT);
			multi_handle = curl_multi_init();
		}
		virtual void deinit_service() noexcept final override
		{
			infrastructure::Logger::get().info("http", "Curl: deinit curl");

			for (const auto& task : tasks)
				close_handle(task.first);
			tasks.clear();

			curl_multi_cleanup(multi_handle);
			multi_handle = nullptr;
			curl_global_cleanup();
		}

		virtual bool can_push_task() const noexcept final override
		{
			return tasks.size() < task_limit;
		}
		virtual void push_task(HttpTaskPtr&& http_task) noexcept final override
		{
			CURL* curl = open_curl_task(std::move(http_task));
			prepare_curl_handle(curl);
			add_to_perform(curl);
		}
		virtual bool pop_completed_task(HttpTaskPtr& http_task) noexcept final override
		{
			if (completed_tasks.empty())
				return false;
			http_task = std::move(completed_tasks.front());
			completed_tasks.pop_front();
			return true;
		}

		virtual bool perform() noexcept final override
		{
			perform_data();
			push_data();
			return not(tasks.empty() and completed_tasks.empty());
		}

	private:
		CURL* open_curl_task(HttpTaskPtr&& http_task) noexcept
		{
			CURL* curl = curl_easy_init();
			tasks[curl] = std::make_unique<CurlTask>();
			tasks[curl]->http_task = std::move(http_task);
			return curl;
		}
		void prepare_curl_handle(CURL* curl) noexcept
		{
			assert(curl != nullptr);

			const CurlTaskPtr& task = tasks[curl];

			const utils::keystring& domain_path = to_string(task->http_task->request.url);
			curl_easy_setopt_wrap(curl, CURLOPT_URL, domain_path.c_str());
			curl_easy_setopt_wrap(curl, CURLOPT_FOLLOWLOCATION, 1);

			if (task->http_task->request.method == Method::POST)
			{
				curl_easy_setopt_wrap(curl, CURLOPT_POST, 1);
				const utils::keystring lenght_header {headers::content_length(task->http_task->request.body.size())};
				task->http_task->request.headers.push_back(lenght_header);
			}

			assert(task->headers == nullptr);
			for (const auto& header : task->http_task->request.headers)
				task->headers = curl_slist_append(task->headers, header.c_str());
			curl_easy_setopt_wrap(curl, CURLOPT_HTTPHEADER, task->headers);

			curl_easy_setopt_wrap(curl, CURLOPT_HEADERFUNCTION, &write_header);
			curl_easy_setopt_wrap(curl, CURLOPT_HEADERDATA, task.get());
			curl_easy_setopt_wrap(curl, CURLOPT_WRITEFUNCTION, &write_data);
			curl_easy_setopt_wrap(curl, CURLOPT_WRITEDATA, task.get());
			curl_easy_setopt_wrap(curl, CURLOPT_READFUNCTION, &read_data);
			curl_easy_setopt_wrap(curl, CURLOPT_READDATA, task.get());

			curl_easy_setopt_wrap(curl, CURLOPT_SSL_CTX_DATA, this);
			curl_easy_setopt_wrap(curl, CURLOPT_SSL_CTX_FUNCTION, &ssl_ctx_callback_wrap);
		}
		void add_to_perform(CURL* curl) noexcept
		{
			assert(curl != nullptr);
			check_code(curl_multi_add_handle(multi_handle, curl), "curl_multi_add_handle");
		}

	private:
		static std::size_t write_header(void* content, std::size_t size, std::size_t nmemb, void* ptr)
		{
			HttpTask* task = static_cast<CurlTask*>(ptr)->http_task.get();
			const char* data = static_cast<char*>(content);
			const std::size_t copy_size = size * nmemb;
			task->response.headers.emplace_back(std::string{data, data+copy_size});
			return copy_size;
		}
		static std::size_t write_data(void* content, std::size_t size, std::size_t nmemb, void* ptr)
		{
			HttpTask* task = static_cast<CurlTask*>(ptr)->http_task.get();
			infrastructure::DataBuffer& body = task->response.body;
			const std::size_t init_size = body.size();
			const std::size_t copy_size = size * nmemb;
			body.resize(init_size + copy_size);
			std::memcpy(body.data() + init_size, content, copy_size);
			return copy_size;
		}
		static std::size_t read_data(void* buffer, std::size_t size, std::size_t nmemb, void* ptr)
		{
			HttpTask* task = static_cast<CurlTask*>(ptr)->http_task.get();
			std::size_t& posted_data_size = static_cast<CurlTask*>(ptr)->posted_data_size;
			infrastructure::DataBuffer& body = task->request.body;
			const std::size_t requested_size = size * nmemb;
			if (requested_size == 0)
				return 0;
			const std::size_t copy_size = std::min(body.size() - posted_data_size, requested_size);
			std::memcpy(buffer, body.data() + posted_data_size, copy_size);
			posted_data_size += copy_size;
			return copy_size;
		}

		static CURLcode ssl_ctx_callback_wrap(CURL* curl, void* ssl_ctx, void* userptr)
		{
			cURLWorker* pthis = static_cast<cURLWorker*>(userptr);
			return pthis->ssl_ctx_callback(curl, ssl_ctx);
		}
		CURLcode ssl_ctx_callback(CURL*, void* ssl_ctx)
		{
			X509_STORE* store = SSL_CTX_get_cert_store(static_cast<SSL_CTX *>(ssl_ctx));

			for (const auto& ca_cert : CurlCAList)
			{
				BIO* bio = BIO_new_mem_buf(const_cast<void*>(static_cast<const void*>(ca_cert.c_str())), -1);
				if (bio == nullptr)
				{
					log_error("BIO_new_mem_buf");
					continue;
				}
				X509 *cert = nullptr;
				PEM_read_bio_X509(bio, &cert, 0, NULL);
				if(cert == NULL)
				{
					log_error("PEM_read_bio_X509");
				}
				else
				{
					if(X509_STORE_add_cert(store, cert) == 0)
						infrastructure::Logger::get().error("http:ssl", "error adding certificate");

					X509_free(cert);
				}
				BIO_free(bio);
			}

			/* all set to go */
			return CURLE_OK;
		}

	private:
		void log_error(const utils::keystring& functionname)
		{
			char buffer[128] = "";
			for (;;)
			{
				const unsigned long err = ERR_get_error();
				if (err == 0)
					break;
				ERR_error_string_n(err, buffer, sizeof(buffer));
				infrastructure::Logger::get().error("http:ssl", "%s failed 0x%x: %s", functionname.c_str(), err, buffer);
			}
		}

	private:
		void perform_data()
		{
			fd_set fdread;
			fd_set fdwrite;
			fd_set fdexcep;

			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcep);

			int maxfd = -1;
			const CURLMcode cc = check_code(curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd), "curl_multi_fdset");
			if (cc != CURLM_OK)
			{
				infrastructure::Logger::get().error("http", "curl_multi_fdset exit with error: %d", static_cast<int>(cc));
				return;
			}

			if (maxfd == -1)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			else
			{
				timeval timeout = get_timeout();
				if (select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout) == -1)
					infrastructure::Logger::get().error("http", "select exit with error: %d", errno);
			}

			int still_running = 0;
			check_code(curl_multi_perform(multi_handle, &still_running), "curl_multi_perform");
		}

		timeval get_timeout() const
		{
			timeval timeout = {0, 100000};
			long curl_timeout = 0;
			check_code(curl_multi_timeout(multi_handle, &curl_timeout), "curl_multi_timeout");
			if (curl_timeout >= 0)
			{
				timeout.tv_sec = curl_timeout / 1000;
				timeout.tv_usec = (curl_timeout % 1000 ) * 1000;
			}
			return timeout;
		}

		void push_data()
		{
			int msg_left = 0;
			while(CURLMsg* msg = curl_multi_info_read(multi_handle, &msg_left))
				if (msg->msg == CURLMSG_DONE)
				{
					on_complete_request(msg);
					CURL* curl = msg->easy_handle;
					close_handle(curl);
					tasks.erase(curl);
				}
		}

		void on_complete_request(CURLMsg* msg)
		{
			const CurlTaskPtr& task = tasks[msg->easy_handle];
			task->http_task->response.error_code = static_cast<ErrorCode>(msg->data.result);
			if (msg->data.result != CURLE_OK)
				infrastructure::Logger::get().error("http", "select exit with error: %d (%s)", static_cast<int>(msg->data.result), curl_easy_strerror(msg->data.result));
			else
			{
				long status_code = static_cast<long>(Status::UNKNOWN);
				check_code(curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &status_code), "CURLINFO_RESPONSE_CODE");
				task->http_task->response.status_code = static_cast<Status>(status_code);
			}
			completed_tasks.emplace_back(std::move(task->http_task));
		}

		void close_handle(CURL* curl)
		{
			check_code(curl_multi_remove_handle(multi_handle, curl), "curl_multi_remove_handle");
			curl_slist_free_all(tasks[curl]->headers);
			curl_easy_cleanup(curl);
		}

		template <typename Param>
		static CURLcode curl_easy_setopt_wrap(CURL* handle, CURLoption option, Param param)
		{
			const CURLcode code = curl_easy_setopt(handle, option, param);
			return check_code(code, "curl_easy_setopt");
		}

		static CURLcode check_code(CURLcode code, const char* function)
		{
			assert(function != nullptr);
			if (code != CURLE_OK)
				infrastructure::Logger::get().warning("http", "Curl: error while execute function: %s; error: %s", function, curl_easy_strerror(code));
			return code;
		}
		static CURLMcode check_code(CURLMcode code, const char* function)
		{
			assert(function != nullptr);
			if (code != CURLM_OK)
				infrastructure::Logger::get().warning("http", "Curl: error while execute function: %s; error: %s", function, curl_multi_strerror(code));
			return code;
		}
	};
}
}
}

