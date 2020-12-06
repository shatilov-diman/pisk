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

#include <pisk/utils/noncopyable.h>

#include "gl/opengl.h"


namespace pisk
{
namespace graphic
{
	class Engine :
		public utils::noncopyable
	{
	public:

		void update()
		{
			glClearColor(0, 0.7f, 0, 0);
			//glClearDepth(1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	};
}//namespace graphic
}//namespace pisk

