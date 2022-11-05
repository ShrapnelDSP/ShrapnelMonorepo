import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../parameter.dart';
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
                        mapping: mapping,
                        onChanged: (value) => midiMappingService.updateMapping(
                          mapping.copyWith.mapping(midiChannel: value!),
                        ),
                      ),
                    ),
                      DataCell(
                        MidiCCDropdown(
                          key: Key('${mapping.id}-cc-number-dropdown'),
                          mapping: mapping,
                          onChanged: (value) =>
                              midiMappingService.updateMapping(
                                mapping.copyWith.mapping(ccNumber: value!),
                          ),
                        ),
                      ),
                      DataCell(
                        ParametersDropdown(
                          key: Key('${mapping.id}-parameter-id-dropdown'),
                          mapping: mapping,
                          onChanged: (value) {
                            midiMappingService.updateMapping(
                                mapping.copyWith.mapping(parameterId: value!),
                            );
                          },
                        ),
                      ),
                    DataCell(IconButton(
                      key: Key('${mapping.id}-delete-button'),
                      icon: const Icon(Icons.delete),
                      onPressed: () => midiMappingService.deleteMapping(id: mapping.id),
                    )),
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
                        id: '123',
                        mapping: MidiMapping(
                          midiChannel: channel!,
                          ccNumber: ccNumber!,
                          parameterId: parameter!,
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
    required this.mapping,
    required this.onChanged,
    super.key,
  });

  final MidiMappingEntry mapping;
  final void Function(int?) onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButton(
      items: List<DropdownMenuItem<int>>.generate(
        16,
        (i) => DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
      ),
      onChanged: onChanged,
      value: mapping.mapping.midiChannel,
    );
  }
}

class MidiCCDropdown extends StatelessWidget {
  const MidiCCDropdown({
    required this.mapping,
    required this.onChanged,
    super.key,
  });

  final MidiMappingEntry mapping;
  final void Function(int?) onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButton(
      items: List<DropdownMenuItem<int>>.generate(
        128,
        (i) => DropdownMenuItem(value: i, child: Text('$i')),
      ),
      onChanged: onChanged,
      value: mapping.mapping.ccNumber,
    );
  }
}

class ParametersDropdown extends StatelessWidget {
  const ParametersDropdown({
    required this.mapping,
    required this.onChanged,
    super.key,
  });

  final MidiMappingEntry mapping;
  final void Function(String?) onChanged;

  @override
  Widget build(BuildContext context) {
    final parameters = context.read<ParameterService>().parameterNames;

    return DropdownButton<String>(
      items: [
        const DropdownMenuItem(child: Text('None')),
        ...parameters.keys.map(
          (id) => DropdownMenuItem(
            value: id,
            child: Text(parameters[id]!),
          ),
        )
      ],
      onChanged: onChanged,
      value: mapping.mapping.parameterId,
    );
  }
}
