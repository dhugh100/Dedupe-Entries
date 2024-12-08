// This file, work_options.h, is a part of the ddup program.
// ddup is a GTK program to find and take action on duplicate files.
// 
// Copyright (C) 2024  David Hugh
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https:www.gnu.org/licenses/>.
#ifndef work_options_h
#define work_options_h

void work_options_cb (GSimpleAction *, GVariant *, user_data *);
int read_options (unsigned char *, char *);

#endif
