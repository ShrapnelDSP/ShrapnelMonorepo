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

class HeavyMetalModel extends StompboxModel {
  HeavyMetalModel()
      : parameters = [
          'heavyMetalLevel',
          'heavyMetalLow',
          'heavyMetalHigh',
          'heavyMetalDistortion',
        ],
        bypass = 'heavyMetalBypass';

  static const _name = 'Heavy Metal';

  @override
  String get name => _name;

  @override
  final List<String> parameters;

  @override
  final String bypass;
}

class HeavyMetal extends StatelessWidget {
  const HeavyMetal({
    super.key,
    required this.full,
    required this.onTap,
  });

  final bool full;
  final void Function() onTap;

  @override
  Widget build(BuildContext context) {
    return Stompbox(
      model: HeavyMetalModel(),
      onCardTap: onTap,
      full: full,
      primarySwatch: Colors.deepOrange,
    );
  }
}
