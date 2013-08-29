/* JNativeHook: Global keyboard and mouse hooking for Java.
 * Copyright (C) 2006-2013 Alexander Barker.  All Rights Received.
 * http://code.google.com/p/jnativehook/
 *
 * JNativeHook is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * JNativeHook is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <nativehook.h>

#include <stdbool.h>
#include <stdio.h>
#include <X11/Xlib.h>

#ifdef USE_XKB
#include <X11/XKBlib.h>
#endif

#ifdef USE_XF86MISC
#include <X11/extensions/xf86misc.h>
#include <X11/extensions/xf86mscstr.h>
#endif

#ifdef USE_XT
#include <X11/Intrinsic.h>
extern Display *xt_disp;
#endif

extern Display *disp;

NATIVEHOOK_API long int hook_get_auto_repeat_rate() {
	bool successful = false;
	long int value = -1;
	unsigned int delay = 0, rate = 0;

	#ifdef USE_XKB
	// Attempt to acquire the keyboard auto repeat rate using the XKB extension.
	if (!successful) {
		successful = XkbGetAutoRepeatRate(disp, XkbUseCoreKbd, &delay, &rate);

		#ifdef USE_DEBUG
		if (successful) {
			fprintf(stdout, "hook_get_auto_repeat_rate(): XkbGetAutoRepeatRate success. (%d)\n", rate);
		}
		#endif
	}
	#endif

	#ifdef USE_XF86MISC
	// Fallback to the XF86 Misc extension if available and other efforts failed.
	if (!successful) {
		XF86MiscKbdSettings kb_info;
		successful = (bool) XF86MiscGetKbdSettings(disp, &kb_info);
		if (successful) {
			#ifdef USE_DEBUG
			fprintf(stdout, "hook_get_auto_repeat_rate(): XF86MiscGetKbdSettings success. (%d)\n", kbdinfo.rate);
			#endif

			delay = (unsigned int) kbdinfo.delay;
			rate = (unsigned int) kbdinfo.rate;
		}
	}
	#endif

	if (successful) {
		value = (long int) rate;
	}

	return value;
}

NATIVEHOOK_API long int hook_get_auto_repeat_delay() {
	bool successful = false;
	long int value = -1;
	unsigned int delay = 0, rate = 0;

	#ifdef USE_XKB
	// Attempt to acquire the keyboard auto repeat rate using the XKB extension.
	if (!successful) {
		successful = XkbGetAutoRepeatRate(disp, XkbUseCoreKbd, &delay, &rate);

		#ifdef USE_DEBUG
		if (successful) {
			fprintf(stdout, "hook_get_auto_repeat_delay(): XkbGetAutoRepeatRate success. (%d)\n", delay);
		}
		#endif
	}
	#endif

	#ifdef USE_XF86MISC
	// Fallback to the XF86 Misc extension if available and other efforts failed.
	if (!successful) {
		XF86MiscKbdSettings kb_info;
		successful = (bool) XF86MiscGetKbdSettings(disp, &kb_info);
		if (successful) {
			#ifdef USE_DEBUG
			fprintf(stdout, "hook_get_auto_repeat_delay(): XF86MiscGetKbdSettings success. (%d)\n", kbdinfo.delay);
			#endif

			delay = (unsigned int) kbdinfo.delay;
			rate = (unsigned int) kbdinfo.rate;
		}
	}
	#endif

	if (successful) {
		value = (long int) delay;
	}

	return value;
}

NATIVEHOOK_API long int hook_get_pointer_acceleration_multiplier() {
	long int value = -1;
	int accel_numerator, accel_denominator, threshold;

	XGetPointerControl(disp, &accel_numerator, &accel_denominator, &threshold);
	if (accel_denominator >= 0) {
		#ifdef USE_DEBUG
		fprintf(stdout, "hook_get_multi_click_time(): XGetPointerControl success. (%d)\n", accel_denominator);
		#endif

		value = (long int) accel_denominator;
	}

	return value;
}

NATIVEHOOK_API long int hook_get_pointer_acceleration_threshold() {
	long int value = -1;
	int accel_numerator, accel_denominator, threshold;

	XGetPointerControl(disp, &accel_numerator, &accel_denominator, &threshold);
	if (threshold >= 0) {
		#ifdef USE_DEBUG
		fprintf(stdout, "hook_get_multi_click_time(): XGetPointerControl success. (%d)\n", threshold);
		#endif

		value = (long int) threshold;
	}

	return value;
}

NATIVEHOOK_API long int hook_get_pointer_sensitivity() {
	long int value = -1;
	int accel_numerator, accel_denominator, threshold;

	XGetPointerControl(disp, &accel_numerator, &accel_denominator, &threshold);
	if (accel_numerator >= 0) {
		#ifdef USE_DEBUG
		fprintf(stdout, "hook_get_multi_click_time(): XGetPointerControl success. (%d)\n", accel_numerator);
		#endif

		value = (long int) accel_numerator;
	}

	return value;
}

NATIVEHOOK_API long int hook_get_multi_click_time() {
	long int value = 200;
	int click_time;
	bool successful = false;

	#ifdef USE_XT
	// Try and use the Xt extention to get the current multi-click.
	if (!successful) {
		// Fall back to the X Toolkit extension if available and other efforts failed.
		click_time = XtGetMultiClickTime(xt_disp);
		if (click_time >= 0) {
			#ifdef USE_DEBUG
			fprintf(stdout, "hook_get_multi_click_time(): XtGetMultiClickTime success. (%d)\n", click_time);
			#endif

			successful = true;
		}
	}
	#endif

	// Try and acquire the multi-click time from the user defined X defaults.
	if (!successful) {
		char *xprop = XGetDefault(disp, "*", "multiClickTime");
		if (xprop != NULL && sscanf(xprop, "%4i", &click_time) != EOF) {
			#ifdef USE_DEBUG
			fprintf(stdout, "hook_get_multi_click_time(): Found multiClickTime property. (%d)\n", click_time);
			#endif

			successful = true;
		}
	}

	if (!successful) {
		char *xprop = XGetDefault(disp, "OpenWindows", "MultiClickTimeout");
		if (xprop != NULL && sscanf(xprop, "%4i", &click_time) != EOF) {
			#ifdef USE_DEBUG
			fprintf(stdout, "hook_get_multi_click_time(): Found MultiClickTimeout property. (%d)\n", click_time);
			#endif

			successful = true;
		}
	}

	if (successful) {
		value = (long int) click_time;
	}

	return value;
}