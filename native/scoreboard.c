#include "scoreboard.h"
#include "course_mgr.h"
#include "tuxracer.h"
#include "gui_label.h"
#include "platform.h"
#include "scoreloop.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_RANK	3
#define MAX_NAME	50
#define MAX_SCORE	10

typedef struct score_item
{
	char rank[MAX_RANK];
	char name[MAX_NAME];
	char score[MAX_SCORE];
}
SCORE_ITEM;

/* our (global) scoreboard */
SCORE_ITEM scoreboard[SCOREBOARD_SIZE];

widget_t* rank_labels[SCOREBOARD_SIZE]={NULL};
widget_t* name_labels[SCOREBOARD_SIZE]={NULL};
widget_t* score_labels[SCOREBOARD_SIZE]={NULL};

bool_t scoreboard_open=False;
bool_t arrays_initialized=False;

unsigned int current_scoreboard=-1;

int stored_score_mode=-1;
int stored_score_value=0;

#ifdef __ANDROID__

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "scoreloop"
#define  LOG(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define JNI(f)	Java_com_moonlite_tuxracer_ ## f

int scoreloop_submit_score(unsigned int scoreMode, unsigned int scoreValue)
{
    JNIEnv* env = Android_JNI_GetEnv();
    if (!env)
    {
        return;
    }
    jclass mActivityClass = (*env)->FindClass(env, "com/moonlite/tuxracer/SDLActivity");
    jmethodID mid = (*env)->GetStaticMethodID(env, mActivityClass, "submitScore", "(II)Z");
    if (!mid)
    {
        return ;
    }
    if (!(*env)->CallStaticBooleanMethod(env, mActivityClass, mid, (int)scoreMode, (int)scoreValue))
    {
        jstring j_title=(*env)->NewStringUTF(env, ALIAS_PROMPT_TITLE);
        jstring j_message=(*env)->NewStringUTF(env, ALIAS_PROMPT_MESSAGE);
        if (!getparam_should_prompt_alias())
        {
            return;
        }
        stored_score_mode=(int)scoreMode;
        stored_score_value=(int)scoreValue;
        env = Android_JNI_GetEnv();
        if (!env)
        {
            return;
        }
        mActivityClass = (*env)->FindClass(env, "com/moonlite/tuxracer/SDLActivity");
        mid = (*env)->GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");
        if (!mid)
        {
            return ;
        }
        (*env)->CallStaticVoidMethod(env, mActivityClass, mid, (int)scoreMode, (int)scoreValue);
        (*env)->DeleteLocalRef(env, j_title);
        (*env)->DeleteLocalRef(env, j_message);
    }
}

JNIEXPORT void JNICALL JNI(SDLActivity_nativeDisableAliasPrompt)(JNIEnv *env, jclass cls)
{
    setparam_should_prompt_alias(False);
}

int scoreloop_refresh_scores(unsigned int scoreMode)
{
    JNIEnv* env = Android_JNI_GetEnv();
    loading_scoreboards();
    if (!env)
    {
        return;
    }
    jclass mActivityClass = (*env)->FindClass(env, "com/moonlite/tuxracer/SDLActivity");
    jmethodID mid = (*env)->GetStaticMethodID(env, mActivityClass, "requestScores", "(I)V");
    if (!mid)
    {
        return ;
    }
    (*env)->CallStaticVoidMethod(env, mActivityClass, mid, (int)scoreMode);
}

JNIEXPORT void JNICALL JNI(SDLActivity_nativeScoreloopGotScores)(JNIEnv *env, jclass cls, jint scoreMode, jobjectArray scoreStrings)
{
    if (scoreMode==current_scoreboard)
    {
        jsize len = (*env)->GetArrayLength(env, scoreStrings);
        jstring tmp_jstring;
        char* score_string_tmp;
        char score_string[100];
        char* first_tab, * second_tab;
        int i;
        char rank_buf[3];
        for (i=0; i<SCOREBOARD_SIZE; i++)
        {
            SCORE_ITEM* score = &scoreboard[i];
            sprintf(rank_buf, "%d", i+1);
            strcpy(score->rank, rank_buf);
            strcpy(score->name, "----");
            strcpy(score->score, "-");
        }
        
        for (i=0; i<len; i++)
        {
            tmp_jstring=(jstring)((*env)->GetObjectArrayElement(env, scoreStrings, i));
            score_string_tmp=(*env)->GetStringUTFChars(env, tmp_jstring, 0);
            strcpy(score_string, score_string_tmp);
            (*env)->ReleaseStringUTFChars(env, tmp_jstring, score_string_tmp);
            
            first_tab=strchr(score_string, '\t');
            second_tab=strchr(first_tab+1, '\t');
            
            *first_tab=*second_tab=0;
            
            strcpy(scoreboard[i].rank, score_string);
            strcpy(scoreboard[i].name, first_tab+1);
            strcpy(scoreboard[i].score, second_tab+1);
        }
        update_scoreboard_labels();
    }
    else
    {
        print_debug(DEBUG_OTHER, "%d, %d", scoreMode, current_scoreboard);
    }
}

#endif


void loading_scoreboards()
{
    unsigned int u;
    for (u = 0; u < SCOREBOARD_SIZE; ++u)
    {
        char buf[3];
        SCORE_ITEM* score = &scoreboard[u];
        sprintf(buf, "%d", u+1);
        strcpy(score->rank, "");
        strcpy(score->name, "");
        strcpy(score->score, "");
    }
    strcpy(scoreboard[4].name, "     Loading...");
    update_scoreboard_labels();
}

void init_scoreboard_arrays()
{
    int course, rank;
    list_t course_list=get_score_courses_list();
    list_elem_t cur_elem=get_list_head(course_list);
    if (!arrays_initialized)
    {
		unsigned int u;
		for (u = 0; u < SCOREBOARD_SIZE; ++u)
		{
			SCORE_ITEM* score = &scoreboard[u];
			memset(score->rank, 0, MAX_RANK);
			memset(score->name, 0, MAX_NAME);
			memset(score->score, 0, MAX_SCORE);
		}
        arrays_initialized=True;
    }
}

void refresh_scores_for_course(const char* course_name)
{
#ifdef __ANDROID__
	current_scoreboard = get_course_index(course_name);
	scoreloop_refresh_scores(current_scoreboard);
#else
#endif
}

void init_scoreboard_labels()
{
	unsigned int u;
	char buf[10];
	widget_t* tmp;
    int course;
	for (u=0; u < SCOREBOARD_SIZE; u++)
	{
		sprintf(buf, "%d", u+1);
		rank_labels[u]=create_label(buf);

		name_labels[u]=create_label("");

		score_labels[u]=create_label("");
	}
    init_scoreboard_arrays();
}

void update_scoreboard_labels()
{
	char buf[10];
	int course;
    unsigned int u;
    if (!scoreboard_open)
    {
        return;
    }
    for (u = 0; u < SCOREBOARD_SIZE; u++)
    {
		const SCORE_ITEM* score = &scoreboard[u];
        button_set_text(rank_labels[u], score->rank);
        button_set_text(name_labels[u], score->name);
		button_set_text(score_labels[u], score->score);
    }
}

widget_t* get_name_label(int rank)
{
	return name_labels[rank];
}

widget_t* get_score_label(int rank)
{
	return score_labels[rank];
}

widget_t* get_rank_label(int rank)
{
	return rank_labels[rank];
}

void submit_score(const char* course_name, int course_score)
{
#ifdef __ANDROID__
	unsigned int courseID = get_course_index(course_name);
	scoreloop_submit_score(courseID, course_score);
#else
	print_debug(DEBUG_OTHER, "");
#endif
}
