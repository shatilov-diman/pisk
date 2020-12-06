// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
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

#include <pisk/model/ReflectedPresentation.h>

namespace pisk
{
namespace model
{
namespace location
{
	template <typename cv_property>
	class PositionPresentationBase:
		public ReflectedItemBase<cv_property>
	{
	public:
		PositionPresentationBase(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}
		PositionPresentationBase<const cv_property> const_ref() const
		{
			return PositionPresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator PositionPresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		ReflectedItemBase<cv_property> x()
		{
			static const utils::keystring kid("x");
			return this->get_double_item(kid);
		}
		ReflectedItemBase<const cv_property> x() const
		{
			return const_ref().x();
		}
		ReflectedItemBase<cv_property> y()
		{
			static const utils::keystring kid("y");
			return this->get_double_item(kid);
		}
		ReflectedItemBase<const cv_property> y() const
		{
			return const_ref().y();
		}
		ReflectedItemBase<cv_property> z()
		{
			static const utils::keystring kid("z");
			return this->get_double_item(kid);
		}
		ReflectedItemBase<const cv_property> z() const
		{
			return const_ref().z();
		}
	};
	using ConstPositionPresentation = PositionPresentationBase<const utils::property>;
	using PositionPresentation = PositionPresentationBase<utils::property>;

	template <typename cv_property>
	class QuaternionPresentationBase:
		public PositionPresentationBase<cv_property>
	{
	public:
		QuaternionPresentationBase(const cv_property& orig, cv_property& prop):
			PositionPresentationBase<cv_property>(orig, prop)
		{}
		QuaternionPresentationBase<const cv_property> const_ref() const
		{
			return QuaternionPresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator QuaternionPresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		ReflectedItemBase<cv_property> w()
		{
			static const utils::keystring kid("w");
			return this->get_double_item(kid);
		}
		ReflectedItemBase<const cv_property> w() const
		{
			return const_ref().w();
		}
	};
	using ConstQuaternionPresentation = QuaternionPresentationBase<const utils::property>;
	using QuaternionPresentation = QuaternionPresentationBase<utils::property>;


	template <typename property>
	using ScalePresentationBase = PositionPresentationBase<property>;

	using ConstScalePresentation = ScalePresentationBase<const utils::property>;
	using ScalePresentation = ScalePresentationBase<utils::property>;

	template <typename cv_property>
	class PresentationBase:
		public ReflectedPresentationBase<cv_property>
	{
	public:
		static const utils::keystring presentation_id()
		{
			return "location";
		}

		PresentationBase(const cv_property& orig, cv_property& prop):
			ReflectedPresentationBase<cv_property>(orig, prop)
		{}
		PresentationBase<const cv_property> const_ref() const
		{
			return PresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator PresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		PositionPresentationBase<cv_property> position()
		{
			static const utils::keystring kid("position");
			return this->properties().template get_custom_item<PositionPresentationBase<cv_property>>(kid);
		}
		PositionPresentationBase<const cv_property> position() const
		{
			return const_ref().position();
		}

		QuaternionPresentationBase<cv_property> quaternion()
		{
			static const utils::keystring kid("quaternion");
			return this->properties().template get_custom_item<QuaternionPresentationBase<cv_property>>(kid);
		}
		QuaternionPresentationBase<const cv_property> quaternion() const
		{
			return const_ref().quaternion();
		}

		ScalePresentationBase<cv_property> scale()
		{
			static const utils::keystring kid("scale");
			return this->properties().template get_custom_item<ScalePresentationBase<cv_property>>(kid);
		}
		ScalePresentationBase<const cv_property> scale() const
		{
			return const_ref().scale();
		}
	};
	using ConstPresentation = PresentationBase<const utils::property>;
	using Presentation = PresentationBase<utils::property>;
}
}
}


