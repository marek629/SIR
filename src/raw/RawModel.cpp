/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2015  Marek Jędryka   <jedryka89@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Program URL: http://marek629.github.io/SIR/
 */

#include "raw/RawModel.hpp"

#include "Settings.hpp"
#include "raw/RawModelValidator.hpp"
#include "raw/RawToolbox.hpp"


RawModel::RawModel() {}

RawModel::RawModel(bool enabled, const QString &dcrawPath,
                   const QString &dcrawOptions)
{
    this->enabled = enabled;
    this->dcrawPath = dcrawPath;
    this->dcrawOptions = dcrawOptions;
}

RawModel::RawModel(const RawModel &other)
{
    this->enabled = other.enabled;
    this->dcrawPath = other.dcrawPath;
    this->dcrawOptions = other.dcrawOptions;
}

RawModel::~RawModel() {}

bool RawModel::isValid() const
{
    return RawModelValidator().isValidModel(*this);
}

void RawModel::load(const Settings &settings)
{
    enabled = settings.value("Raw/enabled", false).toBool();
    dcrawPath = settings.value("Raw/dcrawPath", "/usr/bin/dcraw").toString();
    dcrawOptions = settings.value("Raw/dcrawOptions", "").toString();
}

void RawModel::save(Settings *settings)
{
    settings->beginGroup("Raw");

    settings->setValue("enabled", enabled);
    settings->setValue("dcrawPath", dcrawPath);
    settings->setValue("dcrawOptions", dcrawOptions);

    settings->endGroup();
}

bool RawModel::isEnabled() const
{
    return enabled;
}

void RawModel::setEnabled(bool value)
{
    enabled = value;
}

QString RawModel::getDcrawPath() const
{
    return dcrawPath;
}

void RawModel::setDcrawPath(const QString &value)
{
    dcrawPath = value;
}

QString RawModel::getDcrawOptions() const
{
    return dcrawOptions;
}

void RawModel::setDcrawOptions(const QString &value)
{
    dcrawOptions = value;
}

QString RawModel::fileFilterString()
{
    QString fileFilters;

    if (isEnabled()) {
        foreach (QString ext, RawToolbox(this).fileFilters()) {
            fileFilters.append(ext);
        }
    }

    return fileFilters;
}
