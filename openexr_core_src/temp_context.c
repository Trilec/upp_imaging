/*
** Minimal temp-context bridge for OpenEXRCore probe work.
*/

#include "upstream/openexr_context.h"

#include <stdlib.h>
#include <string.h>

struct _priv_exr_context_t
{
	char* filename;
};

static char*
dup_text(const char* src)
{
	size_t n = src ? strlen(src) : 0;
	char* dst = (char*)malloc(n + 1);
	if(!dst)
		return 0;
	if(n)
		memcpy(dst, src, n);
	dst[n] = 0;
	return dst;
}

exr_result_t exr_start_temporary_context(exr_context_t* ctxt, const char* context_name, const exr_context_initializer_t* ctxtdata)
{
	(void)ctxtdata;
	if(!ctxt)
		return EXR_ERR_INVALID_ARGUMENT;
	*ctxt = 0;
	struct _priv_exr_context_t* c = (struct _priv_exr_context_t*)malloc(sizeof(*c));
	if(!c)
		return EXR_ERR_OUT_OF_MEMORY;
	c->filename = dup_text(context_name ? context_name : "<temporary>");
	if(!c->filename) {
		free(c);
		return EXR_ERR_OUT_OF_MEMORY;
	}
	*ctxt = (exr_context_t)c;
	return EXR_ERR_SUCCESS;
}

exr_result_t exr_get_file_name(exr_const_context_t ctxt, const char** name)
{
	if(!ctxt)
		return EXR_ERR_MISSING_CONTEXT_ARG;
	if(!name)
		return EXR_ERR_INVALID_ARGUMENT;
	*name = ((const struct _priv_exr_context_t*)ctxt)->filename;
	return EXR_ERR_SUCCESS;
}

exr_result_t exr_finish(exr_context_t* pctxt)
{
	struct _priv_exr_context_t* c;
	if(!pctxt)
		return EXR_ERR_MISSING_CONTEXT_ARG;
	c = (struct _priv_exr_context_t*)(*pctxt);
	if(c) {
		free(c->filename);
		free(c);
	}
	*pctxt = 0;
	return EXR_ERR_SUCCESS;
}
