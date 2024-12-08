// This file, get_results.c, is a part of the ddup program.
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

#include "main.h"
#include "get_results.h"

// Comparison function to sort list store by hash
int cmp_function (const void * a, const void * b)
{
	DupItem const *a_item = a;
	DupItem const *b_item = b;
	return (memcmp(a_item->hash, b_item->hash, sizeof(a_item->hash))); // compare hash 
}

// Get group designation into the result field of the item
// - Field could have been initialed with error or directory prior to call

int get_results(user_data *udp) 
{
	// Use quick sort - sort by hash to enable finding dup groups
	g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_function, NULL);

	// String work buff
	char buff[100] = {0x00};

	// Loop through sorted file to get unqiue groups assigned to each file based on hash
	char cghash[65] = "a hash"; // will store current group hash while looping
	memset(&cghash,0x00,sizeof(cghash));
	uint32_t group = 0; // will serve as the group identifier
	uint32_t i = 0; // loop counter
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)); 

	DupItem *item = g_object_new (DUP_TYPE_ITEM, NULL);
	DupItem *next_item = g_object_new (DUP_TYPE_ITEM, NULL);

	// Edge case of 1 entry
	if (cnt == 1) {
		item = g_list_model_get_item((GListModel *)udp->list_store,0);
		if (strlen(item->result) != 0x00) { // Skip if a directory or an error
			g_object_unref(item);
			return 1;
		}
		g_object_set(item, "result", STR_UNI, NULL);
		g_object_unref(item);
		return 1;
	}

	// Main loop to determine groups
	while ( (i+1) < cnt ) { // Since look ahead to determine group, last struct handled outside loop
		
		if (udp->cancel_request == TRUE ) return 0; // Stop processing if cancel requested

		item = g_list_model_get_item((GListModel *)udp->list_store,i);

		// Skip if a directory or an error already in result, otherwise should be 1 or more
		if (strlen(item->result) > 0) {
			i++;
			g_object_unref(item);
			continue; 
		}	

		next_item =  g_list_model_get_item((GListModel *)udp->list_store,i+1);
	
		// See if current item equal to saved current group hash	
		if (!strcmp((const char *)cghash, item->hash)) { // true if not equal to current group 
			sprintf(buff,"%07d",group);
			g_object_set(item, "result", buff, NULL);
		}
		else { // current hash = previous hash
			if (!strcmp(item->hash, next_item->hash)) { // see if next hash =, if so means a group
				group++; // found a group
				sprintf(buff,"%07d",group);
				g_object_set(item, "result", buff, NULL);
				strcpy(cghash, item->hash);
			}
			else {
				g_object_set(item, "result", STR_UNI, NULL);
			}
		}
		i++;
		g_object_unref(item);
		g_object_unref(next_item);
	}

	// Handle last file struct outside of loop
	item = g_list_model_get_item((GListModel *)udp->list_store,i);

	// First see if result already there
	if (strlen(item->result) > 0) { // Skip if a directory or an error
		g_object_unref(item);
		return 1;
	}

	// Create a result
	if ( !strcmp((const char *)cghash, item->hash) ) { // false if equal 
		sprintf(buff,"%07d",group);
		g_object_set(item, "result", buff, NULL);
		g_object_unref(item);
	}
	else {
		g_object_set(item, "result", STR_UNI, NULL);
		g_object_unref(item);
	}
	return 1;
}	
