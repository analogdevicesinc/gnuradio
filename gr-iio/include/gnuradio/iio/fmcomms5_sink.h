/* -*- c++ -*- */
/*
 * Copyright 2015 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS5_SINK_H
#define INCLUDED_IIO_FMCOMMS5_SINK_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_sink.h"
#include "fmcomms2_sink.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific sink for FMComms5 evaluation card
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for FMComms5 evaluation
 * card. The FMComms5 is a dual AD9361 FMC card which enables 4x4
 * applications.
 */

template <typename T>
class IIO_API fmcomms5_sink : public fmcomms2_sink<T>
{
public:
    typedef std::shared_ptr<fmcomms5_sink> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size,
		     bool cyclic);

    virtual void set_frequency(double frequency1, double frequency2) = 0;
};

typedef fmcomms5_sink<int16_t> fmcomms5_sink_s;
typedef fmcomms5_sink<std::complex<int16_t>> fmcomms5_sink_sc16;
typedef fmcomms5_sink<gr_complex> fmcomms5_sink_fc32;

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS5_SINK_H */
