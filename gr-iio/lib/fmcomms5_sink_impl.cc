/* -*- c++ -*- */
/*
 * Copyright 2015 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "device_source_impl.h"
#include "fmcomms5_sink_impl.h"
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/io_signature.h>
#include <ad9361.h>

#include <string>
#include <vector>

using namespace gr::blocks;

namespace gr {
namespace iio {

template <typename T>
typename fmcomms5_sink<T>::sptr fmcomms5_sink<T>::make(const std::string& uri,
                                                       const std::vector<bool>& ch_en,
                                                       unsigned long buffer_size,
                                                       bool cyclic)
{
    return gnuradio::make_block_sptr<fmcomms5_sink_impl<T>>(
        device_source_impl::get_context(uri), ch_en, buffer_size, cyclic);
}

template <>
fmcomms5_sink_impl<int16_t>::fmcomms5_sink_impl(iio_context* ctx,
                                                const std::vector<bool>& ch_en,
                                                unsigned long buffer_size,
                                                bool cyclic)
    : fmcomms2_sink_impl<int16_t>(ctx, ch_en, buffer_size, cyclic)
{
    phy2 = iio_context_find_device(ctx, "ad9361-phy-B");
    if (!phy2)
        throw std::runtime_error("Device not found");
}

template <typename T>
fmcomms5_sink_impl<T>::fmcomms5_sink_impl(iio_context* ctx,
                                          const std::vector<bool>& ch_en,
                                          unsigned long buffer_size,
                                          bool cyclic)
    : fmcomms2_sink_impl<T>(ctx, ch_en, buffer_size, cyclic)
{
    phy2 = iio_context_find_device(ctx, "ad9361-phy-B");
    if (!phy2)
        throw std::runtime_error("Device not found");
}

template <typename T>
fmcomms5_sink_impl<T>::~fmcomms5_sink_impl()
{
}

template <typename T>
void fmcomms5_sink_impl<T>::set_params(const iio_param_vec_t& params)
{
    device_source_impl::set_params(this->phy, params);
    device_source_impl::set_params(this->phy2, params);
}

template <typename T>
void fmcomms5_sink_impl<T>::set_attenuation(size_t chan, double attenuation)
{
    if (chan > 3) {
        throw std::runtime_error("Channel out of range for this device");
    }
    iio_param_vec_t params;
    params.emplace_back("out_voltage" + std::to_string(chan % 2) + "_hardwaregain",
                        -attenuation);

    if (chan < 2)
        device_source_impl::set_params(this->phy, params);
    else
        device_source_impl::set_params(this->phy2, params);
}

template <typename T>
void fmcomms5_sink_impl<T>::set_frequency(double freq1, double freq2)
{
    iio_param_vec_t params1, params2;
    params1.emplace_back("out_altvoltage1_TX_LO_frequency",
                         static_cast<unsigned long long>(freq1));
    params2.emplace_back("out_altvoltage1_TX_LO_frequency",
                         static_cast<unsigned long long>(freq2));

    device_source_impl::set_params(this->phy, params1);
    device_source_impl::set_params(this->phy2, params2);
}

template <typename T>
void fmcomms5_sink_impl<T>::update_dependent_params()
{
    fmcomms2_sink_impl<T>::update_dependent_params();

    ad9361_fmcomms5_multichip_sync(this->ctx, FIXUP_INTERFACE_TIMING | CHECK_SAMPLE_RATES);
}

template class fmcomms5_sink<int16_t>;
template class fmcomms5_sink<std::complex<int16_t>>;
template class fmcomms5_sink<gr_complex>;

} /* namespace iio */
} /* namespace gr */
