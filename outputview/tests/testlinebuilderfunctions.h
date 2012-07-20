/*
    This file is part of KDevelop
    Copyright (C) 2012  Morten Danielsen Volden mvolden2@gmail.com

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
*/
#ifndef TESTLINEBUILDERFUNCTIONS_H
#define TESTLINEBUILDERFUNCTIONS_H

#include <KUrl>

#include <QString>

namespace KDevelop
{

QString buildCppCheckErrorLine()
{
    /// Use existing directory with one file
    KUrl projecturl( PROJECTS_SOURCE_DIR"/onefileproject/" );

    /// Test CPP check output
    QString outputline("[");
    outputline.append(projecturl.path());
    outputline.append("main.cpp:26]: (error) Memory leak: str");
    return outputline;
}

QString buildKrazyErrorLine()
{
    /// Use existing directory with one file
    KUrl projecturl( PROJECTS_SOURCE_DIR"/onefileproject/" );

    /// Test krazy2 output
    QString outputline;
    outputline.append(projecturl.path());
    outputline.append("main.cpp: line#22 (1)");
    return outputline;
}

QString buildCompilerLine()
{
    /// Test with compiler output
    KUrl projecturl( PROJECTS_SOURCE_DIR"/onefileproject/" );
    QString outputline;
    outputline.append(projecturl.path());
    outputline.append(">make");
    return outputline;
}

QString buildCompilerErrorLine()
{
    KUrl projecturl( PROJECTS_SOURCE_DIR"/onefileproject/" );
    QString outputline;
    outputline.clear();
    outputline.append(projecturl.path());
    outputline.append("main.cpp:5:5: error: ‘RingBuffer’ was not declared in this scope");
    return outputline;
}

QString buildCompilerActionLine()
{
    return QString("linking testCustombuild (g++)");
}

QString buildPythonErrorLine()
{
    KUrl projecturl( PROJECTS_SOURCE_DIR"/onefileproject/" );
    QString outputline("File \"");
    outputline.append(projecturl.path());
    outputline.append("pythonExample.py\", line 10");
    return outputline;
}

QString buildCppCheckInformationLine()
{
    return QString("(information) Cppcheck cannot find all the include files. Cpppcheck can check the code without the include\
    files found. But the results will probably be more accurate if all the include files are found. Please check your project's \
    include directories and add all of them as include directories for Cppcheck. To see what files Cppcheck cannot find use --check-config.");
}

}

#endif
