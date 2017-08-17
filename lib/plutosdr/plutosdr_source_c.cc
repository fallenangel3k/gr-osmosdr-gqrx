/* -*- c++ -*- */
/*
 * Copyright 2012 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

#include "arg_helpers.h"
#include "osmosdr/source.h"
#include "plutosdr_source_c.h"

using namespace boost::assign;

plutosdr_source_c_sptr make_plutosdr_source_c(const std::string &args)
{
  return gnuradio::get_initial_sptr(new plutosdr_source_c(args));
}

plutosdr_source_c::plutosdr_source_c(const std::string &args) :
    gr::hier_block2("plutosdr_source_c",
                   gr::io_signature::make(0, 0, 0),
                   gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
  uri = "ip:192.168.2.1";
  //uri = "usb:6.29.5";
  frequency = 434000000;
  samplerate = 2500000;
  decimation = 0;
  bandwidth = 2000000;
  buffer_size = 0x4000;
  quadrature = true;
  rfdc = true;
  bbdc = true;
  gain_mode = "manual";     // FIXME: slow_attack / fast_attack / hybrid
  gain_value = 60;
  filter = "";
  filter_auto = true;

  _src = gr::iio::pluto_source::make(uri.c_str(), frequency, samplerate,
                                     decimation, bandwidth, buffer_size,
                                     quadrature, rfdc, bbdc,
                                     gain_mode.c_str(), gain_value,
                                     filter.c_str(), filter_auto);

  connect( _src, 0, self(), 0 );
}

plutosdr_source_c::~plutosdr_source_c()
{
}

std::vector< std::string > plutosdr_source_c::get_devices()
{
  std::vector< std::string > devices;

  std::string args = "plutosdr,label='PlutoSDR'";

  devices.push_back( args );

  return devices;
}

std::string plutosdr_source_c::name()
{
  return "PlutoSDR";
}

size_t plutosdr_source_c::get_num_channels()
{
  return output_signature()->max_streams();
}

osmosdr::meta_range_t plutosdr_source_c::get_sample_rates( void )
{
  osmosdr::meta_range_t rates;

  rates += osmosdr::range_t( 2500000 );
  rates += osmosdr::range_t( 5000000 );
  rates += osmosdr::range_t( 10000000 );
  rates += osmosdr::range_t( 20000000 );

  return rates;
}

double plutosdr_source_c::set_sample_rate( double rate )
{
  samplerate = (unsigned long) rate;
  set_params();

  return samplerate;
}

double plutosdr_source_c::get_sample_rate( void )
{
  return samplerate;
}

osmosdr::freq_range_t plutosdr_source_c::get_freq_range( size_t chan )
{
  osmosdr::freq_range_t range;

  range += osmosdr::range_t( 70.0e6, 6000.0e6, 1.0 );

  return range;
}

double plutosdr_source_c::set_center_freq( double freq, size_t chan )
{
  frequency = (unsigned long long) freq;
  set_params();

  return freq;
}

double plutosdr_source_c::get_center_freq( size_t chan )
{
  return frequency;
}

double plutosdr_source_c::set_freq_corr( double ppm, size_t chan)
{
  _freq_corr = ppm;
  return ppm;
}

double plutosdr_source_c::get_freq_corr( size_t chan)
{
  return _freq_corr;
}

std::vector<std::string> plutosdr_source_c::get_gain_names( size_t chan )
{
  std::vector< std::string > gains;

  gains.push_back( "Gain" );

  return gains;
}

osmosdr::gain_range_t plutosdr_source_c::get_gain_range( size_t chan)
{
  osmosdr::gain_range_t range;

  range += osmosdr::range_t( 0, 100, 1 );     // FIXME: I have no idea...

  return range;
}

osmosdr::gain_range_t plutosdr_source_c::get_gain_range( const std::string & name,
                                                         size_t chan)
{
  osmosdr::gain_range_t range;

  range += osmosdr::range_t( 0, 100, 1 );     // FIXME: I have no idea...

  return range;
}


double plutosdr_source_c::set_gain( double gain, size_t chan )
{
  gain_value = gain;
  set_params();

  return gain;
}

double plutosdr_source_c::set_gain( double gain, const std::string & name, size_t chan )
{
  gain_value = gain;
  set_params();

  return gain;
}

double plutosdr_source_c::get_gain( size_t chan )
{
  return gain_value;
}

double plutosdr_source_c::get_gain( const std::string & name, size_t chan )
{
  return gain_value;
}

std::vector< std::string > plutosdr_source_c::get_antennas( size_t chan )
{
  std::vector< std::string > antennas;

  antennas += get_antenna( chan );

  return antennas;
}

std::string plutosdr_source_c::set_antenna( const std::string & antenna, size_t chan )
{
  return get_antenna( chan );
}

std::string plutosdr_source_c::get_antenna( size_t chan )
{
  return "A_BALANCED";
}

double plutosdr_source_c::set_bandwidth( double bw, size_t chan )
{
  if (bw == 0.0)
    bw = 0.8 * samplerate;  // auto bandwidth

  bandwidth = (unsigned long)bw;
  set_params();
  return bandwidth;
}

double plutosdr_source_c::get_bandwidth( size_t chan )
{
  return bandwidth;
}

void plutosdr_source_c::set_params( void )
{
  _src->set_params( frequency, samplerate, bandwidth, quadrature, rfdc, bbdc,
                    gain_mode.c_str(), gain_value,
                    filter.c_str(), filter_auto );
}