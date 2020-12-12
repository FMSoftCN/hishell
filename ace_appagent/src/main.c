/////////////////////////////////////////////////////////////////////////////// //
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/**
 \verbatim

    This file is part of HybridOS, a developing operating system based on
    MiniGUI. HybridOs will support embedded systems and smart IoT devices.

    Copyright (C) 2020 Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

#include <stddef.h>
#include <stdio.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL HiAceJsRun(const char* appPath, const char* bundle, const char* fontPath, const char* fontName);

#ifdef __cplusplus
}
#endif


void usage (void)
{
    printf ("usage: ace_appagent appPath bundleName [fontPath] [fontFileName]\n");
}

int MiniGUIMain (int argc, const char* argv[])
{
    if (argc == 1 || argc < 3) {
        usage ();
        return -1;
    }

    const char* appPath = argv[1];
    const char* bundleName = argv[2];

    const char* fontPath = NULL;
    if (argc >= 4)
    {
        fontPath = argv[3];
    }

    const char* fontFileName = NULL;
    if (argc >=5)
    {
        fontFileName = argv[4];
    }

    HiAceJsRun(appPath, bundleName, fontPath, fontFileName);
    return 0;
}
