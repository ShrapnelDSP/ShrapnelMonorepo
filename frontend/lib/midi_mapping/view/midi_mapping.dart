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

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../parameter.dart';
import '../../util/uuid.dart';
import '../model/models.dart';
import '../model/service.dart';

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
              DataColumn(label: Text('Parameter')),
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
                        onChanged: (value) => midiMappingService.updateMapping(
                          mapping.copyWith.mapping(midiChannel: value!),
                        ),
                      ),
                    ),
                    DataCell(
                      MidiCCDropdown(
                        key: Key('${mapping.id}-cc-number-dropdown'),
                        value: mapping.mapping.ccNumber,
                        onChanged: (value) => midiMappingService.updateMapping(
                          mapping.copyWith.mapping(ccNumber: value!),
                        ),
                      ),
                    ),
                    DataCell(
                      ModeDropdown(
                        key: Key('${mapping.id}-mode-dropdown'),
                        value: mapping.mapping.mode,
                        onChanged: (value) => midiMappingService.updateMapping(
                          mapping.copyWith.mapping(mode: value!),
                        ),
                      ),
                    ),
                    DataCell(
                      ParametersDropdown(
                        key: Key('${mapping.id}-parameter-id-dropdown'),
                        value: mapping.mapping.parameterId,
                        onChanged: (value) {
                          midiMappingService.updateMapping(
                            mapping.copyWith.mapping(parameterId: value!),
                          );
                        },
                      ),
                    ),
                    DataCell(
                      IconButton(
                        key: Key('${mapping.id}-delete-button'),
                        icon: const Icon(Icons.delete),
                        onPressed: () =>
                            midiMappingService.deleteMapping(id: mapping.id),
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
        onPressed: () => showDialog<void>(
          context: context,
          builder: (context) => const CreateMappingDialog(),
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
  String? parameter;

  @override
  Widget build(BuildContext context) {
    final parameters = context.read<ParameterService>().parameterNames;
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
                  'Create a new MIDI mapping',
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
                  )
                ],
                value: parameter,
                onChanged: (value) => setState(() => parameter = value),
                validator: validateIsNotNull,
              ),
              ElevatedButton(
                onPressed: () {
                  if (_formKey.currentState!.validate()) {
                    Navigator.pop(context);
                    mappings.createMapping(
                      MidiMappingEntry(
                        id: context.read<Uuid>().v4(),
                        mapping: MidiMapping(
                          midiChannel: channel!,
                          ccNumber: ccNumber!,
                          parameterId: parameter!,
                          mode: mode!,
                        ),
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
      return 'A value must be selected';
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
  final void Function(MidiMappingMode?) onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButton<MidiMappingMode>(
      items: [
        ...MidiMappingMode.values.map(
          (mode) => DropdownMenuItem(
            value: mode,
            child: Text(mode.uiName),
          ),
        )
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
        )
      ],
      onChanged: onChanged,
      value: value,
    );
  }
}
