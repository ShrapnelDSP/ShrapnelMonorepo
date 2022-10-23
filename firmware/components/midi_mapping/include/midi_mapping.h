/** \file
 * Mapping MIDI commands to audio parameter changes.
 *
 * \page ws_api websockets JSON API
 *
 * This page documents the JSON-based message passing API used to communicate
 * between the Flutter frontend and the embedded firmware. The API works by
 * passing messages across a websockets connection. This allows the firmware to
 * send events to the frontend without polling. It is useful e.g. when an audio
 * parameter is updated through MIDI, and the new value should be displayed in
 * the UI.
 *
 * All messages have a string valued member `messageType`, used to identify
 * what operation should be performed.
 *
 * Messages may receive no response, a single response message or multiple
 * responses messages. This is documented separately for each message.
 *
 * A message with `messageType` set to `hello` and a single parameter
 * `example_parameter` with an integer value 1 looks like this:
 *
 * ~~~
 * {
 *   "messageType": "hello",
 *   "example_parameter": 1,
 * }
 * ~~~
 *
 * See also:
 * - \subpage ws_api_parameters
 * - \subpage ws_api_midi
 *
 * \page ws_api_midi MIDI mapping JSON messages
 *
 * - Get list of all mappings
 * - Create a new mapping
 * - Update a mapping
 * - Delete a mapping
 *
 * \todo how to nicely show a JSON object template/schema?
 *
 * ~~~
 * MidiMappingEntry:
 *   id: int
 *   midi_channel: int
 *   cc_number: int
 *   parameter_id: string
 * ~~~
 *
 * <table>
 * <tr><th> Message type <th> Parameters <th> Direction <th> Behaviour <th> Example
 * <tr>
 *   <td> `MidiMap::get::request`
 *   <td> None
 *   <td> UI -> Firmware
 *   <td> The firmware will send a single `MidiMap::get::response` that
 *        includes a list of all midi mappings.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::get::request"
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::get::response`
 *   <td> `mappings` (array of `MidiMappingEntry`): The list of all midi mappings
 *   <td> Firmware -> UI
 *   <td> Sent in response to `MidiMap::get::request`
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::get::response",
 *     "mappings": [
 *       { "id": 0, "midi_channel": 0, "cc_number": 1, "parameter_id": "gain" },
 *       { "id": 1, "midi_channel": 0, "cc_number": 2, "parameter_id": "tone" }
 *     ]
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::create`
 *   <td> `mapping` (`MidiMappingEntry`): The MIDI mapping to create
 *   <td> UI -> Firmware
 *   <td> Firmware creates a new MIDI mapping according to the parameters. The
 *        id field is ignored, and a new ID is assigned by the firmware. TODO:
 *        how does the frontend get to know about the ID, in case it wants to
 *        update this mapping?
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::create",
 *     "mapping": {
 *       "id": 0,
 *       "midi_channel": 0,
 *       "cc_number": 1,
 *       "parameter_id": "gain"
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::update`
 *   <td> `mapping` (`MidiMappingEntry`): The MIDI mapping to update
 *   <td> UI -> Firmware
 *   <td> Firmware updates an existing MIDI mapping according to the parameters.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::update",
 *     "mapping": {
 *       "id": 0,
 *       "midi_channel": 0,
 *       "cc_number": 1,
 *       "parameter_id": "gain"
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::remove`
 *   <td> `id` (`int`): The ID of the MIDI mapping to remove
 *   <td> UI -> Firmware
 *   <td> Firmware removes the specified MIDI mapping .
 *        TODO: If we are using the array index as an ID, this causes all
 *        elements after the removed one to have their IDs changed.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::remove",
 *     "id": 0
 *   }
 *   ~~~
 * </table>
 *
 */
#include <cstdint>
#include <etl/array.h>
#include <etl/string.h>
#include "audio_param.h"

namespace shrapnel {
namespace midi {

using mapping_id_t = int;

struct MappingEntry {
    mapping_id_t id;
    uint8_t midi_channel;
    uint8_t cc_number;
    parameters::id_t parameter_name;
};

class Mapping {
    public:
    Mapping(std::shared_ptr<AudioParameters> param);

    static constexpr std::size_t MAX_COUNT = 128;

    etl::array<MappingEntry, MAX_COUNT> get(MappingEntry mapping);
    void create(MappingEntry mapping);
    void update(MappingEntry mapping);
    void remove(mapping_id_t id);

    /** React to a MIDI message by updating an audio parameter if there is a
     * mapping registered
     */
    void process_message(Message message);
};

}
}
