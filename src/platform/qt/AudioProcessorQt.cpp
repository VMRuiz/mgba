/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "AudioProcessorQt.h"

#include "AudioDevice.h"
#include "LogController.h"

#include <QAudioOutput>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QMediaDevices>
#endif

#include <mgba/core/core.h>
#include <mgba/core/thread.h>

using namespace QGBA;

AudioProcessorQt::AudioProcessorQt(QObject* parent)
	: AudioProcessor(parent)
{
}

void AudioProcessorQt::setInput(std::shared_ptr<CoreController> controller) {
	AudioProcessor::setInput(std::move(controller));
	if (m_device) {
		m_device->setInput(input());
		if (m_audioOutput) {
			m_device->setFormat(m_audioOutput->format());
		}
	}
}

void AudioProcessorQt::stop() {
	if (m_audioOutput) {
		m_audioOutput->stop();
		m_audioOutput.reset();
	}
	if (m_device) {
		m_device.reset();
	}
	AudioProcessor::stop();
}

bool AudioProcessorQt::start() {
	if (!input()) {
		LOG(QT, WARN) << tr("Can't start an audio processor without input");
		return false;
	}

	if (!m_device) {
		m_device = std::make_unique<AudioDevice>(this);
	}

	if (!m_audioOutput) {
		QAudioFormat format;
		format.setSampleRate(m_sampleRate);
		format.setChannelCount(2);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		format.setSampleSize(16);
		format.setCodec("audio/pcm");
		format.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
		format.setSampleType(QAudioFormat::SignedInt);

		m_audioOutput = std::make_unique<QAudioOutput>(format);
		m_audioOutput->setCategory("game");
#else
		format.setSampleFormat(QAudioFormat::Int16);

		QAudioDevice device(QMediaDevices::defaultAudioOutput());
		m_audioOutput = std::make_unique<QAudioSink>(device, format);
		LOG(QT, INFO) << "Audio outputting to " << device.description();
#endif
	}

	if (m_audioOutput->state() == QAudio::SuspendedState) {
		m_audioOutput->resume();
	} else {
		m_device->setInput(input());
		m_device->setFormat(m_audioOutput->format());
		m_audioOutput->start(m_device.get());
	}
	return m_audioOutput->state() == QAudio::ActiveState && m_audioOutput->error() == QAudio::NoError;
}

void AudioProcessorQt::pause() {
	if (m_audioOutput) {
		m_audioOutput->suspend();
	}
}

void AudioProcessorQt::setBufferSamples(int) {
}

void AudioProcessorQt::inputParametersChanged() {
	if (m_device) {
		m_device->setFormat(m_audioOutput->format());
	}
}

void AudioProcessorQt::requestSampleRate(unsigned rate) {
	m_sampleRate = rate;
	if (m_device) {
		QAudioFormat format(m_audioOutput->format());
		format.setSampleRate(rate);
		m_device->setFormat(format);
	}
}

unsigned AudioProcessorQt::sampleRate() const {
	if (!m_audioOutput) {
		return 0;
	}
	return m_audioOutput->format().sampleRate();
}
