/**
 * \file     rv/report.h
 * \brief    reporting state changes and other messages upstream
 * \author   bertw
 */

#pragma once

/// \brief        Report message upstream.
/// \param msg    text message as null terminated string
void report_event(const char *msg);

/// \brief             Report multiple valve states (changes) upstream.
/// \param valve_bits  bits represent valve state. 1=open, 0=close
/// \param valve_mask  bit mask to select valves which states are reported (usually the ones which have changed)
void report_valve_status(uint16_t valve_bits, uint16_t valve_mask);

/// \brief             Report pump status (change) upstream
/// \param is_running  If pump motor is currently running
void report_pump_status(bool is_running);

/// \brief               Report pressure control status (change) upstream.
/// \param pressure_low  true if pressure is low (so the pump needs to run)
void report_pc_status(bool pressure_low);


/// \brief              Report a single valve state (change) upstream.
/// \param valve        Number of valve 0...ValveNumberMax
/// \param is_open      true if open
void report_state_change(int valve, int is_open);

struct Report_Cfg {
  int (*putString)(const char *s);
};

void report_setup(struct Report_Cfg cfg);
