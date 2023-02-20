/* -*- c++ -*- */
/*
 * Copyright 2023 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS5_SOURCE_IMPL_H
#define INCLUDED_IIO_FMCOMMS5_SOURCE_IMPL_H

#include "device_source_impl.h"
#include "fmcomms2_source_impl.h"
#include <gnuradio/iio/fmcomms5_source.h>

#include <string>
#include <vector>

namespace gr {
namespace iio {

template <typename T>
class fmcomms5_source_impl : public fmcomms5_source<T>, public fmcomms2_source_impl<T>
{
private:
    iio_device* phy2;

public:
    fmcomms5_source_impl(iio_context* ctx,
                         const std::vector<bool>& ch_en,
                         unsigned long buffer_size);

    ~fmcomms5_source_impl();

    virtual void set_params(const iio_param_vec_t& params);

    virtual void set_frequency(double frequency1, double frequency2);
    virtual void set_gain_mode(size_t chan, const std::string& mode);
    virtual void set_gain(size_t chan, double gain_value);

    virtual void set_frequency(double frequency) {
    }

    virtual void set_len_tag_key(const std::string& len_tag_key) {
	    fmcomms2_source_impl<T>::set_len_tag_key(len_tag_key);
    }

    virtual void set_samplerate(unsigned long samplerate) {
	    fmcomms2_source_impl<T>::set_samplerate(samplerate);
    }

    virtual void set_quadrature(bool quadrature) {
	    fmcomms2_source_impl<T>::set_quadrature(quadrature);
    }

    virtual void set_rfdc(bool rfdc) {
	    fmcomms2_source_impl<T>::set_rfdc(rfdc);
    }

    virtual void set_bbdc(bool bbdc) {
	    fmcomms2_source_impl<T>::set_bbdc(bbdc);
    }

    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename,
                                   float fpass,
                                   float fstop) {
	    fmcomms2_source_impl<T>::set_filter_params(filter_source,
						       filter_filename,
						       fpass, fstop);
    }

protected:
    virtual void update_dependent_params();

    std::vector<std::string> d_gain_mode = {
        "manual", "manual", "manual", "manual",
        "manual", "manual", "manual", "manual"
    }; // TODO - make these enums
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS5_SOURCE_IMPL_H */
