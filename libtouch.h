#ifndef _LIBTOUCH_H
#define _LIBTOUCH_H
#include <stdint.h>

enum libtouch_action_type {
	/**
	 * Pressing or releasing a finger to or from the touch device.
	 */
	LIBTOUCH_ACTION_TOUCH,
	/**
	 * A difference in the position of the center of the touch group over time.
	 */
	LIBTOUCH_ACTION_MOVE,
	/**
	 * The angle of rotation between each finger and the center of the touch
	 * group changes.
	 */
	LIBTOUCH_ACTION_ROTATE,
	/**
	 * The average distance between each finger and the center of the touch
	 * group changes.
	 */
	LIBTOUCH_ACTION_PINCH,
	/**
	 * No change within the configured thresholds over a certain time frame.
	 */
	LIBTOUCH_ACTION_DELAY,
};

/**
 * Represents a change in the number of touch points in the current touch group.
 * DOWN represents pressing a finger against the touch device, and UP represents
 * removing the finger from the device. Both represents any kind of change.
 */
enum libtouch_touch_mode {
	LIBTOUCH_TOUCH_UP = 1 << 0,
	LIBTOUCH_TOUCH_DOWN = 1 << 1,
};

/**
 * Represents the directions in which a LIBTOUCH_ACTION_MOVE can occur. Both
 * POSITIVE_X and NEGATIVE_X corresponds to movement in either direction along
 * the X axis; the same holds for Y.
 */
enum libtouch_move_dir {
	LIBTOUCH_MOVE_POSITIVE_X = 1 << 0,
	LIBTOUCH_MOVE_POSITIVE_Y = 1 << 1,
	LIBTOUCH_MOVE_NEGATIVE_X = 1 << 2,
	LIBTOUCH_MOVE_NEGATIVE_Y = 1 << 3,
};

/**
 * Represents the direction of rotation in which a LIBTOUCH_ACTION_ROTATE can
 * occur. Both CLOCKWISE and ANTICLOCKWISE corresponds to a rotation in either
 * direction.
 */
enum libtouch_rotate_dir {
	LIBTOUCH_ROTATE_CLOCKWISE = 1 << 0,
	LIBTOUCH_ROTATE_ANTICLOCKWISE = 1 << 1,
};

/**
 * Represents the direction in which a LIBTOUCH_ACTION_PINCH can occur. Both
 * UP and DOWN corresponds to a change of any amount.
 */
enum libtouch_scale_dir {
	LIBTOUCH_PINCH_IN = 1 << 0,
	LIBTOUCH_PINCH_OUT = 1 << 1,
};

/**
 * Represents the internal state. The only holder of state information.
 * 
 */
struct libtouch_engine;

/**
 * Represents a gesture,
 * 
 * a gesture is defined as a sequence of actions
 * declarative, no info of state
 */
struct libtouch_gesture;

/**
 * Represents a part of a gesture.
 * Declarative, no information of state.
 */
struct libtouch_action;

/**
 * A region or other delimited area, within which an action listens,
 * Declarative, no information of state.
 */
struct libtouch_target;

/**
 * Reference to gestures and their progress
 */
struct libtouch_gesture_progress {
	struct libtouch_gesture *gesture;
	double progress;
};

struct libtouch_engine *libtouch_engine_create();

struct libtouch_gesture *libtouch_gesture_create(struct libtouch_engine *engine);

/** 
 * Set a min movement before it starts counting as movement.
 * useful for, for instance long pressing, in case of a not 100% stable finger
 * or to ignore possible miss-swipes 
 */
void libtouch_action_move_tolerance(struct libtouch_action *action, int min);

/**
 * sets move tolerance for all actions of a gesture.
 */
void libtouch_gesture_move_tolerance(struct libtouch_gesture *gesture, int min);

/**
 * and for all gestures
 */
void libtouch_engine_move_tolerance(struct libtouch_engine *engine, int min);


struct libtouch_target *libtouch_target_create(
	struct libtouch_engine *engine, unsigned int x, unsigned int y,
	unsigned int width, unsigned int height);

/**
 * Informs the touch engine of a touch event.
 *
 * timestamp: milliseconds from an arbitrary epoch (e.g. CLOCK_MONOTONIC)
 * slot: the slot of this event (e.g. which finger the event was caused by)
 */
void libtouch_engine_register_touch(
	struct libtouch_engine *engine,
	uint32_t timestamp, int slot, enum libtouch_touch_mode mode,
	unsigned int x, unsigned int y);

/**
 * Informs the touch engine of a touch movement event.
 *
 * timestamp: milliseconds from an arbitrary epoch (e.g. CLOCK_MONOTONIC)
 * slot: the slot of the event (e.g. which finger)
 */
void libtouch_engine_register_move(
	struct libtouch_engine *engine,
	uint32_t timestamp, int slot, enum libtouch_move_dir dir,
	int dx, int dy);


struct libtouch_action *libtouch_gesture_add_touch(
	struct libtouch_gesture *gesture, uint32_t mode);

struct libtouch_action *libtouch_gesture_add_move(
	struct libtouch_gesture *gesture, uint32_t direction);

struct libtouch_action *libtouch_gesture_add_rotate(
	struct libtouch_gesture *gesture, uint32_t direction);

struct libtouch_action *libtouch_gesture_add_pinch(
	struct libtouch_gesture *gesture, uint32_t direction);

struct libtouch_action *libtouch_gesture_add_delay(
	struct libtouch_gesture *gesture, uint32_t duration);



/**
 * Sets the threshold of change for an action to be considered complete. The map
 * of threshold units to action type is as follows:
 *
 * - LIBTOUCH_ACTION_TOUCH:  number of touch points
 * - LIBTOUCH_ACTION_MOVE:   positional units
 * - LIBTOUCH_ACTION_ROTATE: degrees
 * - LIBTOUCH_ACTION_PINCH:  positional units
 * - LIBTOUCH_ACTION_DELAY:  milliseconds (must be positive)
 */
void libtouch_action_set_threshold(
	struct libtouch_action *action, int threshold);

/**
 * Sets a libtouch_target that the action must reach to be considered complete.
 * Valid for LIBTOUCH_ACTION_MOVE,
 * where the movement must finish. Cannot be used together with treshold.
 * 
 * For LIBTOUCH_ACTION_TOUCH, target defines where we must press.
 */
void libtouch_action_set_target(
	struct libtouch_action *action,
	struct libtouch_target *target);

/**
 * Sets the minimum duration this action must take place during to be considered
 * a match. For instance, if not all n fingers are pressed the same frame,
 * we can consider n fingers down within duration_ms to be a n-finger touch.
 */
void libtouch_action_set_duration(
	struct libtouch_action *action,
	uint32_t duration_ms);



/**
 * Gets the current progress of this action between 0 and 1.
 */
double libtouch_action_get_progress(
	struct libtouch_engine *engine,
	struct libtouch_action *action);


/** Returns the progress of this gesture from 0..1. */
double libtouch_gesture_get_progress(
	struct libtouch_engine *engine,
	struct libtouch_gesture *gesture);

void libtouch_gesture_reset_progress(
	struct libtouch_engine *engine,
	struct libtouch_gesture *gesture);

/** Returns the active action for this gesture. */
struct libtouch_action *libtouch_gesture_get_current_action(
	struct libtouch_engine *engine,
	struct libtouch_gesture *gesture);

/**
 * Fills an array of libtouch_gesture_progress pointers
 * sorted by progress.
 */
double libtouch_fill_progress_array(
	struct libtouch_engine *engine,
	struct libtouch_gesture_progress **array,
	uint32_t count);

/**
 * Returns a completed gesture, and resets its progress
 * if none exist, return NULL.
 *
 * Call repeatedly to get all finished gestures.
 */
struct libtouch_gesture *libtouch_handle_finished_gesture(
	struct libtouch_engine *engine);


#endif
