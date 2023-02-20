/* -*- c++ -*- */
/*
 * Copyright 2023 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS5_SOURCE_H
#define INCLUDED_IIO_FMCOMMS5_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_source.h"
#include "fmcomms2_source.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific source for the FMComms5 evaluation card
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for FMComms5 evaluation
 * card. The FMComms5 is a dual AD9361 FMC card which enables 4x4
 * applications.
 */
template <typename T>
class IIO_API fmcomms5_source : public fmcomms2_source<T>
{
public:
    typedef std::shared_ptr<fmcomms5_source> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size);

    virtual void set_frequency(double frequency1, double frequency2) = 0;
};

typedef fmcomms5_source<int16_t> fmcomms5_source_s;
typedef fmcomms5_source<std::complex<int16_t>> fmcomms5_source_sc16;
typedef fmcomms5_source<gr_complex> fmcomms5_source_fc32;

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS5_SOURCE_H */
