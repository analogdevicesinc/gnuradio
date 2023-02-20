/* -*- c++ -*- */
/*
 * Copyright 2015 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS5_SINK_IMPL_H
#define INCLUDED_IIO_FMCOMMS5_SINK_IMPL_H

#include "device_sink_impl.h"
#include "fmcomms2_sink_impl.h"
#include <gnuradio/iio/fmcomms5_sink.h>

#include <string>
#include <vector>

namespace gr {
namespace iio {

template <class T>
class fmcomms5_sink_impl : public fmcomms5_sink<T>, public fmcomms2_sink_impl<T>
{
private:
    iio_device* phy2;

public:
    fmcomms5_sink_impl(iio_context* ctx,
                       const std::vector<bool>& ch_en,
                       unsigned long buffer_size,
                       bool cyclic);

    ~fmcomms5_sink_impl();

    virtual void set_params(const iio_param_vec_t& params);

    virtual void set_frequency(double frequency1, double frequency2);
    virtual void set_attenuation(size_t chan, double gain);

    virtual void set_len_tag_key(const std::string& len_tag_key) {
	    fmcomms2_sink_impl<T>::set_len_tag_key(len_tag_key);
    }

    virtual void set_bandwidth(unsigned long bandwidth) {
	    fmcomms2_sink_impl<T>::set_bandwidth(bandwidth);
    }

    virtual void set_rf_port_select(const std::string& rf_port_select) {
	    fmcomms2_sink_impl<T>::set_rf_port_select(rf_port_select);
    }

    virtual void set_frequency(double frequency) {
    }

    virtual void set_samplerate(unsigned long samplerate) {
	    fmcomms2_sink_impl<T>::set_samplerate(samplerate);
    }

    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0) {
	    fmcomms2_sink_impl<T>::set_filter_params(filter_source,
						     filter_filename,
						     fpass, fstop);
    }

protected:
    virtual void update_dependent_params();
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_IMPL_H */
