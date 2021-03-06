/*
 Copyright (c) 2014 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <joystick.h>
#include <report.h>
#include <controller2.h>

static const char *joystick_axis_name[AXIS_MAX] =
{
  [jsa_lstick_x] = "lstick x",
  [jsa_lstick_y] = "lstick y",
  [jsa_rstick_x] = "rstick x",
  [jsa_rstick_y] = "rstick y",
  [jsa_B8] = "select",
  [jsa_B9] = "start",
  [jsa_up] = "up",
  [jsa_right] = "right",
  [jsa_down] = "down",
  [jsa_left] = "left",
  [jsa_B3] = "triangle",
  [jsa_B2] = "circle",
  [jsa_B1] = "cross",
  [jsa_B0] = "square",
  [jsa_B4] = "l1",
  [jsa_B5] = "r1",
  [jsa_B6] = "l2",
  [jsa_B7] = "r2",
  [jsa_B10] = "l3",
  [jsa_B11] = "r3",
};

static s_axis_name_dir axis_names[] =
{
  {.name = "rstick x",     {.axis = jsa_rstick_x, .props = AXIS_PROP_CENTERED}},
  {.name = "rstick y",     {.axis = jsa_rstick_y, .props = AXIS_PROP_CENTERED}},
  {.name = "lstick x",     {.axis = jsa_lstick_x, .props = AXIS_PROP_CENTERED}},
  {.name = "lstick y",     {.axis = jsa_lstick_y, .props = AXIS_PROP_CENTERED}},

  {.name = "rstick left",  {.axis = jsa_rstick_x, .props = AXIS_PROP_CENTERED | AXIS_PROP_NEGATIVE}},
  {.name = "rstick right", {.axis = jsa_rstick_x, .props = AXIS_PROP_CENTERED | AXIS_PROP_POSITIVE}},
  {.name = "rstick up",    {.axis = jsa_rstick_y, .props = AXIS_PROP_CENTERED | AXIS_PROP_NEGATIVE}},
  {.name = "rstick down",  {.axis = jsa_rstick_y, .props = AXIS_PROP_CENTERED | AXIS_PROP_POSITIVE}},

  {.name = "lstick left",  {.axis = jsa_lstick_x, .props = AXIS_PROP_CENTERED | AXIS_PROP_NEGATIVE}},
  {.name = "lstick right", {.axis = jsa_lstick_x, .props = AXIS_PROP_CENTERED | AXIS_PROP_POSITIVE}},
  {.name = "lstick up",    {.axis = jsa_lstick_y, .props = AXIS_PROP_CENTERED | AXIS_PROP_NEGATIVE}},
  {.name = "lstick down",  {.axis = jsa_lstick_y, .props = AXIS_PROP_CENTERED | AXIS_PROP_POSITIVE}},

  {.name = "select",       {.axis = jsa_B8,       .props = AXIS_PROP_TOGGLE}},
  {.name = "start",        {.axis = jsa_B9,       .props = AXIS_PROP_TOGGLE}},
  {.name = "up",           {.axis = jsa_up,       .props = AXIS_PROP_TOGGLE}},
  {.name = "down",         {.axis = jsa_down,     .props = AXIS_PROP_TOGGLE}},
  {.name = "right",        {.axis = jsa_right,    .props = AXIS_PROP_TOGGLE}},
  {.name = "left",         {.axis = jsa_left,     .props = AXIS_PROP_TOGGLE}},
  {.name = "r1",           {.axis = jsa_B5,       .props = AXIS_PROP_TOGGLE}},
  {.name = "r2",           {.axis = jsa_B7,       .props = AXIS_PROP_TOGGLE}},
  {.name = "r3",           {.axis = jsa_B11,      .props = AXIS_PROP_TOGGLE}},
  {.name = "l1",           {.axis = jsa_B4,       .props = AXIS_PROP_TOGGLE}},
  {.name = "l2",           {.axis = jsa_B6,       .props = AXIS_PROP_TOGGLE}},
  {.name = "l3",           {.axis = jsa_B10,      .props = AXIS_PROP_TOGGLE}},
  {.name = "circle",       {.axis = jsa_B2,       .props = AXIS_PROP_TOGGLE}},
  {.name = "square",       {.axis = jsa_B0,       .props = AXIS_PROP_TOGGLE}},
  {.name = "cross",        {.axis = jsa_B1,       .props = AXIS_PROP_TOGGLE}},
  {.name = "triangle",     {.axis = jsa_B3,       .props = AXIS_PROP_TOGGLE}},
};

static int joystick_max_unsigned_axis_value[AXIS_MAX] =
{
  [jsa_lstick_x] = MAX_AXIS_VALUE_16BITS,
  [jsa_lstick_y] = MAX_AXIS_VALUE_16BITS,
  [jsa_rstick_x] = MAX_AXIS_VALUE_16BITS,
  [jsa_rstick_y] = MAX_AXIS_VALUE_16BITS,
  [jsa_up] = MAX_AXIS_VALUE_8BITS,
  [jsa_right] = MAX_AXIS_VALUE_8BITS,
  [jsa_down] = MAX_AXIS_VALUE_8BITS,
  [jsa_left] = MAX_AXIS_VALUE_8BITS,
  [jsa_B0] = MAX_AXIS_VALUE_8BITS,
  [jsa_B1] = MAX_AXIS_VALUE_8BITS,
  [jsa_B2] = MAX_AXIS_VALUE_8BITS,
  [jsa_B3] = MAX_AXIS_VALUE_8BITS,
  [jsa_B4] = MAX_AXIS_VALUE_8BITS,
  [jsa_B5] = MAX_AXIS_VALUE_8BITS,
  [jsa_B6] = MAX_AXIS_VALUE_8BITS,
  [jsa_B7] = MAX_AXIS_VALUE_8BITS,
  [jsa_B8] = MAX_AXIS_VALUE_8BITS,
  [jsa_B9] = MAX_AXIS_VALUE_8BITS,
  [jsa_B10] = MAX_AXIS_VALUE_8BITS,
  [jsa_B11] = MAX_AXIS_VALUE_8BITS,
  [jsa_B12] = MAX_AXIS_VALUE_8BITS,
};

static s_controller_params joystick_params =
{
    .min_refresh_period = 1000,
    .default_refresh_period = 4000,
    .max_unsigned_axis_value = joystick_max_unsigned_axis_value
};

static unsigned int joystick_report_build(int axis[AXIS_MAX], s_report* report)
{
  s_report_joystick* js = &report->value.js;

  js->X = clamp(0, axis[jsa_lstick_x] + 32768, 65535);
  js->Y = clamp(0, axis[jsa_lstick_y] + 32768, 65535);
  js->Z = clamp(0, axis[jsa_rstick_x] + 32768, 65535);
  js->Rz = clamp(0, axis[jsa_rstick_y] + 32768, 65535);

  js->Bt = 0x0000;

  if (axis[jsa_B0])
  {
    js->Bt |= 0x0001;
  }
  if (axis[jsa_B1])
  {
    js->Bt |= 0x0002;
  }
  if (axis[jsa_B2])
  {
    js->Bt |= 0x0004;
  }
  if (axis[jsa_B3])
  {
    js->Bt |= 0x0008;
  }

  if (axis[jsa_B4])
  {
    js->Bt |= 0x0010;
  }
  if (axis[jsa_B5])
  {
    js->Bt |= 0x0020;
  }
  if (axis[jsa_B6])
  {
    js->Bt |= 0x0040;
  }
  if (axis[jsa_B7])
  {
    js->Bt |= 0x0080;
  }

  if (axis[jsa_B8])
  {
    js->Bt |= 0x0100;
  }
  if (axis[jsa_B9])
  {
    js->Bt |= 0x0200;
  }
  if (axis[jsa_B10])
  {
    js->Bt |= 0x0400;
  }
  if (axis[jsa_B11])
  {
    js->Bt |= 0x0800;
  }

  if (axis[jsa_B12])
  {
    js->Bt |= 0x1000;
  }

  if (axis[jsa_right])
  {
    if (axis[jsa_down])
    {
      js->Hat = 0x0003;
    }
    else if (axis[jsa_up])
    {
      js->Hat = 0x0001;
    }
    else
    {
      js->Hat = 0x0002;
    }
  }
  else if (axis[jsa_left])
  {
    if (axis[jsa_down])
    {
      js->Hat = 0x0005;
    }
    else if (axis[jsa_up])
    {
      js->Hat = 0x0007;
    }
    else
    {
      js->Hat = 0x0006;
    }
  }
  else if (axis[jsa_down])
  {
    js->Hat = 0x0004;
  }
  else if (axis[jsa_up])
  {
    js->Hat = 0x0000;
  }
  else
  {
    js->Hat = 0x0008;
  }

  return sizeof(*js);
}

void joystick_init(void) __attribute__((constructor (101)));
void joystick_init(void)
{
  controller_register_axis_names(C_TYPE_JOYSTICK, sizeof(axis_names)/sizeof(*axis_names), axis_names);

  controller_register_params(C_TYPE_JOYSTICK, &joystick_params);

  control_register_names(C_TYPE_JOYSTICK, joystick_axis_name);

  report_register_builder(C_TYPE_JOYSTICK, joystick_report_build);
}
