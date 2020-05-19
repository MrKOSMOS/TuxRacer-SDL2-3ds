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

#if defined( HAVE_SDL )
#   include "SDL.h"
#endif

#if defined( HAVE_GL_GLX_H )
#   include <GL/glx.h>
#endif /* defined( HAVE_GL_GLX_H ) */

#include "gl_util.h"
#include "shaders.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static glm::mat4 model, view, projection;

void set_gl_options( RenderMode mode ) 
{
    /* Must set the following options:
         Enable/Disable:
	   GL_TEXTURE_2D
	   GL_DEPTH_TEST
	   GL_CULL_FACE
	   GL_LIGHTING
	   GL_NORMALIZE
	   GL_ALPHA_TEST
	   GL_BLEND
	   GL_STENCIL_TEST
	   GL_COLOR_MATERIAL
           
	 Other Functions:
	   glDepthMask
	   glShadeModel
	   glDepthFunc
    */

    /*
     * Modify defaults based on rendering mode
     * 
     * This could could be improved if it stored state and avoided
     * redundant state changes, which are costly (or so I've heard)...  
     */
    switch( mode ) {
    case GUI:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
        break;
    case GAUGE_BARS:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );

        break;

    case TEXFONT:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
        break;

    case TEXT:
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
        break;

    case SPLASH_SCREEN:
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
        break;

    case COURSE:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LEQUAL );

	break;

    case TREES:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );

        break;
        
    case PARTICLES:
        glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );

        break;
        
    case PARTICLE_SHADOWS:
        glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );

        break;

    case SKY:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LESS );
	break;
 	
    case FOG_PLANE:
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	break;

    case TUX:
        glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );

        break;

    case TUX_SHADOW:
#ifdef USE_STENCIL_BUFFER
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glEnable( GL_STENCIL_TEST );
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LESS );

	glStencilFunc( GL_EQUAL, 0, ~0 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
#else
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
#endif
	break;

    case TRACK_MARKS:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	break;

    case OVERLAYS:
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
 
        break;

    default:
	code_not_reached();
    } 
}

void set_MVP()
{
    //eye_pos
    glm::vec4 eye_pos=view*glm::vec4(0, 0, 0, 1);
    glUniformMatrix4fv(shader_get_uniform_location("MVP_mat"), 1, GL_FALSE, glm::value_ptr(projection*view*model));
    glUniform3f(shader_get_uniform_location("eye_pos"), eye_pos[0], eye_pos[1], eye_pos[2]);
}

void util_set_view(float* mat)
{
    int x, y;
    for (x=0; x<4; x++)
    {
        for (y=0; y<4; y++)
        {
            view[x][y]=mat[x*4+y];
        }
    }
    set_MVP();
}

void util_setup_projection(float near, float far)
{
    projection=glm::perspective(getparam_fov() * 3.14159f / 180.0f, (float)getparam_x_resolution()/getparam_y_resolution(), near, far);
    
    set_MVP();
}

void util_set_translation(float x, float y, float z)
{
    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    
    set_MVP();
}

void util_set_model_matrix(float* mat)
{
    int x, y;
    for (x=0; x<4; x++)
    {
        for (y=0; y<4; y++)
        {
            model[x][y]=mat[x*4+y];
        }
    }
    set_MVP();
}

/* Checking for GL errors is really just another type of assertion, so we
   turn off the check if TUXRACER_NO_ASSERT is defined */
#if defined( TUXRACER_NO_ASSERT )
void check_gl_error()
{
}
#else 
void check_gl_error()
{
    GLenum error;
    error = glGetError();
    if ( error != GL_NO_ERROR ) {
	print_warning( CRITICAL_WARNING, 
		       "OpenGL Error: %d", error
               );
	fflush( stderr );
    }
}
#endif /* defined( TUXRACER_NO_ASSERT ) */

void copy_to_glfloat_array( GLfloat dest[], scalar_t src[], int n )
{
    int i;
    for (i=0; i<n; i++) {
	dest[i] = src[i];
    }
}

void init_glfloat_array( int num, GLfloat arr[], ... )
{
    int i;
    va_list args;

    va_start( args, arr );

    for (i=0; i<num; i++) {
	arr[i] = va_arg(args, double);
    }

    va_end( args );
}

typedef void (*(*get_gl_proc_fptr_t)(const GLubyte *))(); 

void init_opengl_extensions()
{
    get_gl_proc_fptr_t get_gl_proc;

    get_gl_proc = NULL;
    {
	print_debug( DEBUG_GL_EXT, 
		     "No function available for obtaining GL proc addresses" );
    }
}



/*---------------------------------------------------------------------------*/
/*! 
  Prints information about the current OpenGL implemenation.
  \author  jfpatry
  \date    Created:  2000-10-20
  \date    Modified: 2000-10-20
*/
typedef struct {
    char *name;
    GLenum value;
    GLenum type;
} gl_value_t;

#ifdef GL_INT
#undef GL_INT
#endif
#define GL_INT GL_FIXED

/* Add more things here as needed */
gl_value_t gl_values[] = {
    { "max texture size", GL_MAX_TEXTURE_SIZE, GL_INT },
    { "red bits", GL_RED_BITS, GL_INT },
    { "green bits", GL_GREEN_BITS, GL_INT },
    { "blue bits", GL_BLUE_BITS, GL_INT },
    { "alpha bits", GL_ALPHA_BITS, GL_INT },
    { "depth bits", GL_DEPTH_BITS, GL_INT },
    { "stencil bits", GL_STENCIL_BITS, GL_INT } };

#ifdef TARGET_OS_IPHONE
// FIXME
# undef GL_INT
#endif

void print_gl_info()
{
    char *extensions;
    char *p, *oldp;
    int i;
    GLint int_val;
    GLfloat float_val;
    GLboolean boolean_val;

    print_debug(DEBUG_OTHER,
	     "  vendor: %s\n", 
	     glGetString( GL_VENDOR ) );

    print_debug(DEBUG_OTHER,
	     "  renderer: %s\n", 
	     glGetString( GL_RENDERER ) );

    print_debug(DEBUG_OTHER,
	     "  version: %s\n", 
	     glGetString( GL_VERSION ) );

    extensions = string_copy( (char*) glGetString( GL_EXTENSIONS ) );

    print_debug(DEBUG_OTHER, "  extensions:\n" );

    oldp = extensions;
    while ( (p=strchr(oldp,' ')) ) {
	*p='\0';
	print_debug(DEBUG_OTHER, "    %s\n", oldp );
	oldp = p+1;
    }
    if ( *oldp ) {
	print_debug(DEBUG_OTHER, "    %s\n", oldp );
    }

    free( extensions );

    for ( i=0; i<sizeof(gl_values)/sizeof(gl_values[0]); i++) {
	print_debug(DEBUG_OTHER, "  %s: ", gl_values[i].name );

	switch( gl_values[i].type ) {
	case GL_FIXED:
	    glGetIntegerv( gl_values[i].value, &int_val );
	    print_debug(DEBUG_OTHER, "%d", int_val );
	    break;
	    glGetIntegerv( gl_values[i].value, &int_val );
	    print_debug(DEBUG_OTHER, "%d", int_val );
	    break;
	case GL_FLOAT:
	    glGetFloatv( gl_values[i].value, &float_val );
	    print_debug(DEBUG_OTHER, "%f", float_val );
	    break;

	case GL_UNSIGNED_BYTE:
	    glGetBooleanv( gl_values[i].value, &boolean_val );
	    print_debug(DEBUG_OTHER, "%d", boolean_val );
	    break;

	default:
	    code_not_reached();
	}

	print_debug(DEBUG_OTHER, "\n" );
    }


    print_debug(DEBUG_OTHER, "\n" );
}
