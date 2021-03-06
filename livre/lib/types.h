/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                          Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of Livre <https://github.com/BlueBrain/Livre>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _livreTypes_h_
#define _livreTypes_h_

#include <livre/core/types.h>

namespace livre
{
class HistogramObject;
class RenderPipeline;
class DataObject;
class TextureObject;
class VolumeRendererParameters;

struct ApplicationParameters;

typedef std::shared_ptr<const DataObject> ConstDataObjectPtr;
typedef std::shared_ptr<const TextureObject> ConstTextureObjectPtr;
typedef std::shared_ptr<const HistogramObject> ConstHistogramObjectPtr;

template <class T>
inline void addOption(options_description& set, const std::string& shortDesc,
                      const std::string& longDesc, const T defaultValue)
{
    set.add_options()(shortDesc.c_str(),
                      boost::program_options::value<T>()->default_value(
                          defaultValue),
                      longDesc.c_str());
}

template <class T>
inline void addOption(options_description& set, const std::string& shortDesc,
                      const std::string& longDesc, const T defaultValue,
                      const T implicitValue)
{
    set.add_options()(shortDesc.c_str(), boost::program_options::value<T>()
                                             ->default_value(defaultValue)
                                             ->implicit_value(implicitValue),
                      longDesc.c_str());
}

inline void addOption(options_description& set, const std::string& shortDesc,
                      const std::string& longDesc, const bool defaultValue)
{
    set.add_options()(shortDesc.c_str(),
                      boost::program_options::bool_switch()->default_value(
                          defaultValue),
                      longDesc.c_str());
}
}

#endif // _types_h_
