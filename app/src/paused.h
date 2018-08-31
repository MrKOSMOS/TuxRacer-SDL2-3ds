/*
 * Tux Racer
 * Copyright (C) 1999-2001 Jasmin F. Patry
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _PAUSED_H_
#define _PAUSED_H_

void paused_init();
void paused_loop(scalar_t time_step);
void paused_register();
void come_back_to_game();
void force_pause_for_ticks(int ticks);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
