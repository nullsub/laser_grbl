
TODO
=====

- In limits.c why are we using the pull-up resistors?




Notes about process flow
========================

The main function inits several interrupts and then goes into an infinite loop processing data from the serial buffer.

- main()                                              --- main.c
  - inits serial interrupt
    - this is what keeps reading incoming serial data and 
      writes it into the serial buffer
  - protocol_process() in an infinite loop            --- protocol.c
    - settings_execute_line(line)                     --- settings.c
      - settings_dump()
      - settings_store_setting(parameter, value)
        - write_settings()
          - memcpy_to_eeprom_with_checksum(...)       --- eeprom.c
            - eeprom_put_char(...)
    - protocol_execute_line()
      - gc_execute_line(line)                         --- gcode.c
        - mc_go_home()                                --- motion_control.c
          - st_go_home()
					  - limits_go_home()
					  - plan_set_current_position(0,0,0)
        - mc_dwell(secs)
          - st_synchronize()
            - plan_get_current_block()
            - sleep_mode()
          - _delay_ms(millisecs)
        - mc_set_current_position(x,y,z)
          - plan_set_current_position(x,y,z)
        - mc_cancel()
          - plan_buffer_command(TYPE_CANCEL)
						- plan_reset_block_buffer()
						- st_go_idle()
						  - set_laser_intensity(LASER_OFF)
						  - _delay_ms(millisecs)
						  - disable stepper inerrupt
        - mc_get_actual_position(&x, &y, &z)
          - st_get_position(&x, &y, &z)
        - mc_airgas_disable()
          - plan_buffer_command(TYPE_AIRGAS_DISABLE)
        - mc_air_enable()
          - plan_buffer_command(TYPE_AIR_ENABLE)
        - mc_gas_enable()
          - plan_buffer_command(TYPE_GAS_ENABLE)
        - mc_arc(...)
          - plan_buffer_line(...)
        - mc_line(...)
          - plan_buffer_line(...)


- plan_buffer_command(...)                            --- planner.c
  - adds a simple command to the stepper command buffer
  - wake up the stepper interrupt (if not already processing)
- plan_buffer_line(...)
  - adds a seek/feed command to the stepper buffer
  - wake up the stepper interrupt (if not already processing)
  - planner_recalculate()
		- planner_reverse_pass();
		- planner_forward_pass();
		- planner_recalculate_trapezoids()



Notes on AVR-specific code
===========================

When porting to the ARM platform all the standard C code should run fine. Still much of the hardware access is specific to the AVR platform and the Atmega328 chip in particular.

- persistent storage
  - EEPROM becomes flash memory
- serial communication/interrupt
  - serial.c serial.h
- timer interrupt setup/adjustment
  - stepper timer
  - laser pwm


Possible ARM Bugs 
------------------

if (current->entry_speed != current->vmax_junction) {  
... in reverse pass kernel
... vmax_junction used to be called max_entry_speed
