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


#include "tuxracer.h"
#include "textures.h"
#include "image.h"
#include "hash.h"
#include "list.h"

static bool_t initialized = False;
static hash_table_t texture_table;
static hash_table_t binding_table;


bool_t get_texture_binding( const char *binding, GLuint *texid )
{
    texture_node_t *texnode;
    if (get_hash_entry(binding_table, binding, (hash_entry_t*)(&texnode))) {
	*texid = texnode->texture_id;
	return True;
    }
    return False;  
}

bool_t load_and_bind_texture( const char *binding, const char *filename )
{
    return (bool_t) ( load_texture( binding, filename, 1 ) &&
		      bind_texture( binding, binding ) );
}

void init_textures() 
{
    if (!initialized) {
	texture_table = create_hash_table();
	binding_table = create_hash_table();
    }
    initialized = True;
} 

int get_min_filter()
{
    switch( getparam_mipmap_type() ) {
    case 0: 
	return GL_NEAREST;
    case 1:
	return GL_LINEAR;
    case 2: 
	return GL_NEAREST_MIPMAP_NEAREST;
    case 3: 
	return GL_LINEAR_MIPMAP_NEAREST;
    case 4: 
	return GL_NEAREST_MIPMAP_LINEAR;
    case 5: 
	return GL_LINEAR_MIPMAP_LINEAR;
    default:
	return GL_LINEAR_MIPMAP_NEAREST;
    }
}

bool_t load_texture( const char *texname, const char *filename, int repeatable )
{
    SDL_Surface* texImage;
    texture_node_t *tex;
    int max_texture_size;
    int format=0;
    
    print_debug(DEBUG_TEXTURE, "Loading texture %s from file: %s",
                texname, filename);
    if ( initialized == False ) {
        check_assertion( 0, "texture module not initialized" );
    }
    
    texImage = ImageLoad( filename );
    
    if ( texImage == NULL ) {
    	print_warning( IMPORTANT_WARNING,
                      "couldn't load image %s", filename );
    	return False;
    }
    
    if (get_hash_entry( texture_table, texname, (hash_entry_t*)&tex )) {
        print_debug(DEBUG_TEXTURE, "Found texture %s with id: %d",
                    texname, tex->texture_id);
        glDeleteTextures( 1, &(tex->texture_id) );
    } else {
        tex = (texture_node_t*)malloc(sizeof(texture_node_t));
        
        check_assertion( tex != NULL, "out of memory" );
        
        tex->ref_count = 0;
        add_hash_entry( texture_table, texname, (hash_entry_t)tex );
    }
    
    tex->repeatable = repeatable;
    glGenTextures( 1, &(tex->texture_id) );
    glBindTexture( GL_TEXTURE_2D, tex->texture_id );
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    
    /* Check if we need to scale image */
    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &max_texture_size );
	if ( texImage->w > max_texture_size ||
        texImage->h > max_texture_size )
    {
        abort(); //We don't support that yet
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    
	switch (texImage->format->BytesPerPixel)
	{
	case 3:
		format=GL_RGB;
		break;
	case 4:
		format=GL_RGBA;
		break;
	default:
		handle_error(1, "unsupported number of bytes per pixel in texture %s", filename);
		break;
	}

	SDL_LockSurface(texImage);

	glTexImage2D( GL_TEXTURE_2D, 0, format, texImage->w,texImage->h,
                 0, format, GL_UNSIGNED_BYTE, texImage->pixels );

    glGenerateMipmap(GL_TEXTURE_2D);
    
	SDL_UnlockSurface(texImage);
	SDL_FreeSurface(texImage);
    return True;
}

bool_t 
get_texture( const char *texname, texture_node_t **tex )
{
    return get_hash_entry(texture_table, texname, (hash_entry_t*)tex);		
}


bool_t 
del_texture( const char *texname )
{
    texture_node_t *tex;

    print_debug( DEBUG_TEXTURE, "Deleting texture %s", texname );

    if (del_hash_entry(texture_table, texname, (hash_entry_t*)(&tex))) {
	check_assertion( tex->ref_count == 0,
			 "Trying to delete texture with non-zero reference "
			 "count" );
	glDeleteTextures( 1, &(tex->texture_id) );
	free(tex);
	return True;
    }

    return False;
}


bool_t bind_texture( const char *binding, const char *texname )
{
    texture_node_t *tex, *oldtex;

    print_debug(DEBUG_TEXTURE, "Binding %s to texture name: %s", 
		binding, texname);
    if (!get_texture( texname, &tex)) {
	check_assertion(0, "Attempt to bind to unloaded texture");
	return False;
    }

    if (get_hash_entry(binding_table, binding, (hash_entry_t*)(&oldtex))) {
	oldtex->ref_count--;
	if (!del_hash_entry(binding_table, binding, NULL)) {
	    check_assertion(0, "Cannot delete known texture");
	    return False;
	}
    }

    add_hash_entry(binding_table, binding, (hash_entry_t)tex);
    tex->ref_count++;

    return True;
}

bool_t unbind_texture( const char *binding )
{
    texture_node_t *tex;

    if (get_hash_entry( binding_table, binding, (hash_entry_t*)(&tex))) {
	tex->ref_count--;
	if (!del_hash_entry( binding_table, binding, NULL )) {
	    check_assertion(0, "Cannot delete known texture binding");
	    return False;
	}
	return True;
    }

    return False;
}

void get_current_texture_dimensions( int *width, int *height )
{
    abort();
}

bool_t flush_textures(void)
{
    texture_node_t *tex;
    hash_search_t  sptr;
    list_t delete_list;
    list_elem_t elem;
    char *key;
    bool_t result;

    delete_list = create_list();
  
    begin_hash_scan(texture_table, &sptr);
    while ( next_hash_entry(sptr, &key, (hash_entry_t*)(&tex)) ) {
	if (tex->ref_count == 0) {
	    elem = insert_list_elem(delete_list, NULL, (list_elem_data_t)key);
	}
    }
    end_hash_scan(sptr);

    elem = get_list_head(delete_list);
    while (elem != NULL) {
	key = (char*)get_list_elem_data(elem);

	result = del_texture( key );
	check_assertion(result, "Attempt to flush non-existant texture");

	elem = get_next_list_elem(delete_list, elem);
    }

    del_list(delete_list);

    return True;

}

static int load_texture_cb ( ClientData cd, Tcl_Interp *ip, int argc,
			     const char *argv[]) 
{
    int repeatable = 1;

    if ( ( argc != 3 ) && (argc != 4) ) {
	Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], "<texture name> <image file>",
			 " [repeatable]", (char *)0 );
	return TCL_ERROR;
    } 

    if ( ( argc == 4 ) && ( Tcl_GetInt( ip, argv[3], &repeatable ) != TCL_OK ) ) {
        Tcl_AppendResult(ip, argv[0], ": invalid repeatable flag",
			 (char *)0 );
        return TCL_ERROR;
    } 
    
    if (!load_texture(argv[1], argv[2], repeatable)) {
	Tcl_AppendResult(ip, argv[0], ": Could not load texture ", 
			 argv[2], (char*)0);
	return TCL_ERROR;
    }

    return TCL_OK;
}

static int bind_texture_cb ( ClientData cd, Tcl_Interp *ip, int argc, 
			     const char *argv[])
{
    if ( argc != 3 ) {
	Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], "<object name> <texture name>",
			 (char *)0 );
	return TCL_ERROR;
    } 

    if (!bind_texture(argv[1], argv[2])) {
	Tcl_AppendResult(ip, argv[0], ": Could not bind texture ", 
			 argv[2], (char*)0);
	return TCL_ERROR;
    }

    return TCL_OK;
}


void register_texture_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_load_texture",   load_texture_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_bind_texture",   bind_texture_cb,   0,0);
}
