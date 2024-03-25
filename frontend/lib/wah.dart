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

import 'stompbox.dart';

class WahModel extends StompboxModel {
  // ignore: avoid_unused_constructor_parameters
  WahModel()
      : parameters = [
          'wahPosition',
          'wahVocal',
        ],
        bypass = 'wahBypass';

  static const _name = 'Wah';

  @override
  String get name => _name;

  @override
  final String bypass;

  @override
  final List<String> parameters;
}

class Wah extends StatelessWidget {
  const Wah({
    super.key,
    required this.full,
    required this.onTap,
  });

  final bool full;
  final void Function() onTap;

  @override
  Widget build(BuildContext context) {
    return Stompbox(
      model: WahModel(),
      onCardTap: onTap,
      full: full,
      primarySwatch: Colors.purple,
    );
  }
}
