/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

import 'dart:async';

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../parameter.dart';
import '../../presets/model/presets.dart';
import '../model/models.dart';
import '../model/service.dart';

const _requiredValueString = 'A value must be selected';

class MidiMappingPage extends StatelessWidget {
  const MidiMappingPage({super.key});

  @override
  Widget build(BuildContext context) {
    final midiMappingService = context.watch<MidiMappingService>();

    return Scaffold(
      appBar: AppBar(
        title: const Text('MIDI Mapping'),
      ),
      body: Scrollbar(
        child: SingleChildScrollView(
          primary: true,
          scrollDirection: Axis.horizontal,
          child: DataTable(
            columns: const [
              DataColumn(label: Text('MIDI channel')),
              DataColumn(label: Text('CC number')),
              DataColumn(label: Text('Mode')),
              DataColumn(label: Text('Target')),
              DataColumn(label: Text('Delete')),
            ],
            rows: midiMappingService.mappings.entries.map(
              (mapEntry) {
                final mapping = MidiMappingEntry(
                  id: mapEntry.key,
                  mapping: mapEntry.value,
                );
                return DataRow(
                  key: ValueKey(mapping.id),
                  cells: [
                    DataCell(
                      MidiChannelDropdown(
                        key: Key('${mapping.id}-midi-channel-dropdown'),
                        value: mapping.mapping.midiChannel,
                        onChanged: (value) => unawaited(
                          midiMappingService.updateMapping(
                            mapping.copyWith.mapping(midiChannel: value!),
                          ),
                        ),
                      ),
                    ),
                    DataCell(
                      MidiCCDropdown(
                        key: Key('${mapping.id}-cc-number-dropdown'),
                        value: mapping.mapping.ccNumber,
                        onChanged: (value) => unawaited(
                          midiMappingService.updateMapping(
                            mapping.copyWith.mapping(ccNumber: value!),
                          ),
                        ),
                      ),
                    ),
                    DataCell(
                      ModeDropdown(
                        key: Key('${mapping.id}-mode-dropdown'),
                        value: mapping.mapping.mode,
                        onChanged: (value) {
                          if (value == mapping.mapping.mode) {
                            return;
                          }

                          if (value == null) {
                            return;
                          }

                          const parameterModes = [
                            MidiMappingMode.parameter,
                            MidiMappingMode.toggle,
                          ];

                          final isTrivialChange =
                              parameterModes.contains(mapping.mapping.mode) &&
                                  parameterModes.contains(value);

                          if (isTrivialChange) {
                            unawaited(
                              midiMappingService.updateMapping(
                                switch (mapping.mapping) {
                                  final MidiMappingToggle toggle => switch (
                                        value) {
                                      MidiMappingMode.toggle =>
                                        throw StateError(''),
                                      MidiMappingMode.parameter =>
                                        mapping.copyWith(
                                          mapping: MidiMapping.parameter(
                                            midiChannel: toggle.midiChannel,
                                            ccNumber: toggle.ccNumber,
                                            parameterId: toggle.parameterId,
                                          ),
                                        ),
                                      MidiMappingMode.button =>
                                        throw StateError(''),
                                    },
                                  final MidiMappingParameter parameter =>
                                    switch (value) {
                                      MidiMappingMode.toggle =>
                                        mapping.copyWith(
                                          mapping: MidiMapping.toggle(
                                            midiChannel: parameter.midiChannel,
                                            ccNumber: parameter.ccNumber,
                                            parameterId: parameter.parameterId,
                                          ),
                                        ),
                                      MidiMappingMode.parameter =>
                                        throw StateError(''),
                                      MidiMappingMode.button =>
                                        throw StateError(''),
                                    },
                                  MidiMappingButton() => throw StateError(''),
                                },
                              ),
                            );
                          } else {
                            unawaited(
                              showDialog<void>(
                                context: context,
                                builder: (context) => EditMappingDialog(
                                  mapping: mapping,
                                  mode: value,
                                ),
                              ),
                            );
                          }
                        },
                      ),
                    ),
                    DataCell(
                      switch (mapping.mapping) {
                        final MidiMappingToggle midiMapping => Row(
                            children: [
                              Text(
                                'Parameter:',
                                style: Theme.of(context).textTheme.titleMedium,
                              ),
                              const SizedBox(width: 8),
                              ParametersDropdown(
                                key: Key('${mapping.id}-parameter-id-dropdown'),
                                value: midiMapping.parameterId,
                                onChanged: (value) => unawaited(
                                  midiMappingService.updateMapping(
                                    mapping.copyWith(
                                      mapping: midiMapping.copyWith(
                                        parameterId: value!,
                                      ),
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                        final MidiMappingParameter midiMapping => Row(
                            children: [
                              Text(
                                'Parameter:',
                                style: Theme.of(context).textTheme.titleMedium,
                              ),
                              const SizedBox(width: 8),
                              ParametersDropdown(
                                key: Key('${mapping.id}-parameter-id-dropdown'),
                                value: midiMapping.parameterId,
                                onChanged: (value) => unawaited(
                                  midiMappingService.updateMapping(
                                    mapping.copyWith(
                                      mapping: midiMapping.copyWith(
                                        parameterId: value!,
                                      ),
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                        final MidiMappingButton midiMapping => Row(
                            children: [
                              Text(
                                'Preset:',
                                style: Theme.of(context).textTheme.titleMedium,
                              ),
                              const SizedBox(width: 8),
                              PresetsDropdown(
                                key: Key('${mapping.id}-preset-id-dropdown'),
                                value: midiMapping.presetId,
                                onChanged: (value) => unawaited(
                                  midiMappingService.updateMapping(
                                    mapping.copyWith(
                                      mapping: midiMapping.copyWith(
                                        presetId: value!,
                                      ),
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                      },
                    ),
                    DataCell(
                      IconButton(
                        key: Key('${mapping.id}-delete-button'),
                        icon: const Icon(Icons.delete),
                        onPressed: () => unawaited(
                          midiMappingService.deleteMapping(id: mapping.id),
                        ),
                      ),
                    ),
                  ],
                );
              },
            ).toList(growable: false),
          ),
        ),
      ),
      floatingActionButton: FloatingActionButton(
        child: const Icon(Icons.add),
        onPressed: () => unawaited(
          showDialog<void>(
            context: context,
            builder: (context) => const CreateMappingDialog(),
          ),
        ),
      ),
    );
  }
}

class CreateMappingDialog extends StatefulWidget {
  const CreateMappingDialog({super.key});

  @override
  State<StatefulWidget> createState() {
    return CreateMappingDialogState();
  }
}

class CreateMappingDialogState extends State<CreateMappingDialog> {
  final _formKey = GlobalKey<FormState>();
  int? channel;
  int? ccNumber;
  MidiMappingMode? mode;
  int? presetId;
  String? parameterId;

  @override
  Widget build(BuildContext context) {
    final parameters = context.read<ParameterService>().parameterNames;
    final presets = context.watch<PresetsState>();
    final mappings = context.read<MidiMappingService>();

    return Dialog(
      child: Padding(
        padding: const EdgeInsets.all(8),
        child: Form(
          key: _formKey,
          child: Wrap(
            alignment: WrapAlignment.center,
            runSpacing: 8,
            children: [
              Padding(
                padding: const EdgeInsets.all(8),
                child: Text(
                  'Create MIDI mapping',
                  style: Theme.of(context).textTheme.titleLarge,
                ),
              ),
              DropdownButtonFormField<int>(
                decoration: const InputDecoration(
                  label: Text('MIDI channel'),
                ),
                items: List<DropdownMenuItem<int>>.generate(
                  16,
                  (i) =>
                      DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
                ),
                value: channel,
                onChanged: (value) => setState(() => channel = value),
                validator: validateIsNotNull,
              ),
              DropdownButtonFormField<int>(
                decoration: const InputDecoration(
                  label: Text('CC number'),
                ),
                items: List<DropdownMenuItem<int>>.generate(
                  16,
                  (i) =>
                      DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
                ),
                value: ccNumber,
                onChanged: (value) => setState(() => ccNumber = value),
                validator: validateIsNotNull,
              ),
              DropdownButtonFormField<MidiMappingMode>(
                decoration: const InputDecoration(
                  label: Text('Mode'),
                ),
                items: List<DropdownMenuItem<MidiMappingMode>>.generate(
                  MidiMappingMode.values.length,
                  (i) => DropdownMenuItem(
                    value: MidiMappingMode.values[i],
                    child: Text(MidiMappingMode.values[i].uiName),
                  ),
                ),
                value: mode,
                onChanged: (value) => setState(() => mode = value),
                validator: validateIsNotNull,
              ),
              DropdownButtonFormField<String>(
                decoration: const InputDecoration(
                  label: Text('Parameter'),
                ),
                items: [
                  ...parameters.keys.map(
                    (id) => DropdownMenuItem(
                      value: id,
                      child: Text(parameters[id]!),
                    ),
                  ),
                ],
                value: parameterId,
                onChanged: switch (mode) {
                  MidiMappingMode.toggle ||
                  MidiMappingMode.parameter =>
                    (value) => setState(() => parameterId = value),
                  null || MidiMappingMode.button => null,
                },
                validator: (v) {
                  return switch (mode) {
                    null || MidiMappingMode.button => null,
                    MidiMappingMode.toggle ||
                    MidiMappingMode.parameter =>
                      v == null ? _requiredValueString : null,
                  };
                },
              ),
              DropdownButtonFormField<int>(
                decoration: const InputDecoration(
                  label: Text('Preset'),
                ),
                items: switch (presets) {
                  LoadingPresetsState() => null,
                  ReadyPresetsState(:final presets) => presets.map(
                      (e) => DropdownMenuItem(
                        value: e.id,
                        child: Text(e.preset.name),
                      ),
                    ),
                }
                    ?.toList(),
                value: presetId,
                onChanged: switch (mode) {
                  null ||
                  MidiMappingMode.toggle ||
                  MidiMappingMode.parameter =>
                    null,
                  MidiMappingMode.button => (value) =>
                      setState(() => presetId = value),
                },
                validator: (v) {
                  return switch (mode) {
                    null ||
                    MidiMappingMode.toggle ||
                    MidiMappingMode.parameter =>
                      null,
                    MidiMappingMode.button =>
                      v == null ? _requiredValueString : null,
                  };
                },
              ),
              ElevatedButton(
                onPressed: () {
                  if (_formKey.currentState!.validate()) {
                    Navigator.pop(context);
                    unawaited(
                      mappings.createMapping(
                          switch (mode) {
                            null =>
                              throw StateError('Mode has not been selected'),
                            MidiMappingMode.toggle => MidiMapping.toggle(
                                midiChannel: channel!,
                                ccNumber: ccNumber!,
                                parameterId: parameterId!,
                              ),
                            MidiMappingMode.parameter => MidiMapping.parameter(
                                midiChannel: channel!,
                                ccNumber: ccNumber!,
                                parameterId: parameterId!,
                              ),
                            MidiMappingMode.button => MidiMapping.button(
                                midiChannel: channel!,
                                ccNumber: ccNumber!,
                                presetId: presetId!,
                              ),
                          },
                        ),
                    );
                  }
                },
                child: const Text('Create'),
              ),
            ],
          ),
        ),
      ),
    );
  }

  String? validateIsNotNull<T>(T? value) {
    if (value == null) {
      return _requiredValueString;
    }

    return null;
  }
}

class EditMappingDialog extends StatefulWidget {
  const EditMappingDialog({
    super.key,
    required this.mapping,
    this.channel,
    this.ccNumber,
    this.mode,
    this.presetId,
    this.parameterId,
  });

  final MidiMappingEntry mapping;

  final int? channel;
  final int? ccNumber;
  final MidiMappingMode? mode;
  final int? presetId;
  final String? parameterId;

  @override
  State<StatefulWidget> createState() {
    return EditMappingDialogState();
  }
}

class EditMappingDialogState extends State<EditMappingDialog> {
  final _formKey = GlobalKey<FormState>();
  int? channel;
  int? ccNumber;
  MidiMappingMode? mode;
  int? presetId;
  String? parameterId;

  @override
  void initState() {
    super.initState();

    // Let the widget specify an override for some settings, e.g. in case the
    // dialog is shown in response to the user adjusting a dropdown and
    // selecting something that requires more setup.
    channel = widget.channel;
    ccNumber = widget.ccNumber;
    mode = widget.mode;
    presetId = widget.presetId;
    parameterId = widget.parameterId;

    // If no overrides were specified, copy the data from the existing mapping.
    channel ??= widget.mapping.mapping.midiChannel;
    ccNumber ??= widget.mapping.mapping.ccNumber;
    mode ??= widget.mapping.mapping.mode;
    presetId ??= switch (widget.mapping.mapping) {
      MidiMappingToggle() => null,
      MidiMappingParameter() => null,
      MidiMappingButton(:final presetId) => presetId,
    };
    parameterId ??= switch (widget.mapping.mapping) {
      MidiMappingToggle(:final parameterId) => parameterId,
      MidiMappingParameter(:final parameterId) => parameterId,
      MidiMappingButton() => null,
    };
  }

  @override
  Widget build(BuildContext context) {
    final parameters = context.read<ParameterService>().parameterNames;
    final presets = context.watch<PresetsState>();
    final mappings = context.read<MidiMappingService>();

    return Dialog(
      child: Padding(
        padding: const EdgeInsets.all(8),
        child: Form(
          key: _formKey,
          child: Wrap(
            alignment: WrapAlignment.center,
            runSpacing: 8,
            children: [
              Padding(
                padding: const EdgeInsets.all(8),
                child: Text(
                  'Edit MIDI mapping',
                  style: Theme.of(context).textTheme.titleLarge,
                ),
              ),
              DropdownButtonFormField<int>(
                decoration: const InputDecoration(
                  label: Text('MIDI channel'),
                ),
                items: List<DropdownMenuItem<int>>.generate(
                  16,
                  (i) =>
                      DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
                ),
                value: channel,
                onChanged: (value) => setState(() => channel = value),
                validator: validateIsNotNull,
              ),
              DropdownButtonFormField<int>(
                decoration: const InputDecoration(
                  label: Text('CC number'),
                ),
                items: List<DropdownMenuItem<int>>.generate(
                  16,
                  (i) =>
                      DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
                ),
                value: ccNumber,
                onChanged: (value) => setState(() => ccNumber = value),
                validator: validateIsNotNull,
              ),
              DropdownButtonFormField<MidiMappingMode>(
                decoration: const InputDecoration(
                  label: Text('Mode'),
                ),
                items: List<DropdownMenuItem<MidiMappingMode>>.generate(
                  MidiMappingMode.values.length,
                  (i) => DropdownMenuItem(
                    value: MidiMappingMode.values[i],
                    child: Text(MidiMappingMode.values[i].uiName),
                  ),
                ),
                value: mode,
                onChanged: (value) => setState(() => mode = value),
                validator: validateIsNotNull,
              ),
              DropdownButtonFormField<String>(
                decoration: const InputDecoration(
                  label: Text('Parameter'),
                ),
                items: [
                  ...parameters.keys.map(
                    (id) => DropdownMenuItem(
                      value: id,
                      child: Text(parameters[id]!),
                    ),
                  ),
                ],
                value: parameterId,
                onChanged: switch (mode) {
                  MidiMappingMode.toggle ||
                  MidiMappingMode.parameter =>
                    (value) => setState(() => parameterId = value),
                  null || MidiMappingMode.button => null,
                },
                validator: (v) {
                  return switch (mode) {
                    null || MidiMappingMode.button => null,
                    MidiMappingMode.toggle ||
                    MidiMappingMode.parameter =>
                      v == null ? _requiredValueString : null,
                  };
                },
              ),
              DropdownButtonFormField<int>(
                decoration: const InputDecoration(
                  label: Text('Preset'),
                ),
                items: switch (presets) {
                  LoadingPresetsState() => null,
                  ReadyPresetsState(:final presets) => presets.map(
                      (e) => DropdownMenuItem(
                        value: e.id,
                        child: Text(e.preset.name),
                      ),
                    ),
                }
                    ?.toList(),
                value: presetId,
                onChanged: switch (mode) {
                  null ||
                  MidiMappingMode.toggle ||
                  MidiMappingMode.parameter =>
                    null,
                  MidiMappingMode.button => (value) =>
                      setState(() => presetId = value),
                },
                validator: (v) {
                  return switch (mode) {
                    null ||
                    MidiMappingMode.toggle ||
                    MidiMappingMode.parameter =>
                      null,
                    MidiMappingMode.button =>
                      v == null ? _requiredValueString : null,
                  };
                },
              ),
              ElevatedButton(
                onPressed: () {
                  if (_formKey.currentState!.validate()) {
                    Navigator.pop(context);
                    unawaited(
                      mappings.updateMapping(
                        MidiMappingEntry(
                          id: widget.mapping.id,
                          mapping: switch (mode) {
                            null =>
                              throw StateError('Mode has not been selected'),
                            MidiMappingMode.toggle => MidiMapping.toggle(
                                midiChannel: channel!,
                                ccNumber: ccNumber!,
                                parameterId: parameterId!,
                              ),
                            MidiMappingMode.parameter => MidiMapping.parameter(
                                midiChannel: channel!,
                                ccNumber: ccNumber!,
                                parameterId: parameterId!,
                              ),
                            MidiMappingMode.button => MidiMapping.button(
                                midiChannel: channel!,
                                ccNumber: ccNumber!,
                                presetId: presetId!,
                              ),
                          },
                        ),
                      ),
                    );
                  }
                },
                child: const Text('Update'),
              ),
            ],
          ),
        ),
      ),
    );
  }

  String? validateIsNotNull<T>(T? value) {
    if (value == null) {
      return _requiredValueString;
    }

    return null;
  }
}

class MidiChannelDropdown extends StatelessWidget {
  const MidiChannelDropdown({
    required this.value,
    required this.onChanged,
    super.key,
  });

  final int value;
  final void Function(int?) onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButton(
      items: List<DropdownMenuItem<int>>.generate(
        16,
        (i) => DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
      ),
      onChanged: onChanged,
      value: value,
    );
  }
}

class MidiCCDropdown extends StatelessWidget {
  const MidiCCDropdown({
    required this.value,
    required this.onChanged,
    super.key,
  });

  final int value;
  final void Function(int?) onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButton(
      items: List<DropdownMenuItem<int>>.generate(
        128,
        (i) => DropdownMenuItem(value: i, child: Text('$i')),
      ),
      onChanged: onChanged,
      value: value,
    );
  }
}

class ModeDropdown extends StatelessWidget {
  const ModeDropdown({
    required this.value,
    required this.onChanged,
    super.key,
  });

  final MidiMappingMode value;
  final void Function(MidiMappingMode?)? onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButton<MidiMappingMode>(
      items: [
        ...MidiMappingMode.values.map(
          (mode) => DropdownMenuItem(
            value: mode,
            child: Text(mode.uiName),
          ),
        ),
      ],
      onChanged: onChanged,
      value: value,
    );
  }
}

class ParametersDropdown extends StatelessWidget {
  const ParametersDropdown({
    required this.value,
    required this.onChanged,
    super.key,
  });

  final String value;
  final void Function(String?) onChanged;

  @override
  Widget build(BuildContext context) {
    final parameters = context.read<ParameterService>().parameterNames;

    return DropdownButton<String>(
      items: [
        ...parameters.keys.map(
          (id) => DropdownMenuItem(
            value: id,
            child: Text(parameters[id]!),
          ),
        ),
      ],
      onChanged: onChanged,
      value: value,
    );
  }
}

class PresetsDropdown extends StatelessWidget {
  const PresetsDropdown({
    required this.value,
    required this.onChanged,
    super.key,
  });

  final int value;
  final void Function(int?) onChanged;

  @override
  Widget build(BuildContext context) {
    final presets = switch (context.watch<PresetsState>()) {
      ReadyPresetsState(:final presets) => presets,
      LoadingPresetsState() => <PresetRecord>[],
    }
        .map((e) => (e.id, e.preset.name))
        .toList();

    if (presets.every((element) => element.$1 != value)) {
      presets.add((value, '<unknown>'));
    }

    return DropdownButton<int>(
      items: [
        ...presets.map(
          (preset) => DropdownMenuItem(
            value: preset.$1,
            child: Text(preset.$2),
          ),
        ),
      ],
      onChanged: onChanged,
      value: value,
    );
  }
}

extension on MidiMapping {
  MidiMappingMode get mode => switch (this) {
        MidiMappingToggle() => MidiMappingMode.toggle,
        MidiMappingParameter() => MidiMappingMode.parameter,
        MidiMappingButton() => MidiMappingMode.button,
      };
}
