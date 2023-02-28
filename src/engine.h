/* vim:set et sts=4: */
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_WHISPER_ENGINE	\
	(ibus_whisper_engine_get_type ())

GType   ibus_whisper_engine_get_type    (void);

#endif // __ENGINE_H__
