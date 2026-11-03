#ifndef __INCLUDE_SLIDER_H__
#define __INCLUDE_SLIDER_H__

#define FILTER_EN 0

typedef struct
{
	const float x_voltage;
	const float y_voltage;
		
	void (*reset)(void);
	void (*stop)(void);
}Slider;

Slider* getSliderHandle(void);

#endif /* __INCLUDE_SLIDER_H__ */
