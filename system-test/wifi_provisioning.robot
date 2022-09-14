*** Settings ***

Library  Dialogs

*** Variables ***

${MESSAGE_NOT_CONNECTED_ERROR}  Connection failed. Please ensure
    ...                         you are connected to the ShrapnelDSP access
    ...                         point and try again.


*** Test Cases ***

Valid Provisioning
    Given firmware is waiting for provisioning
    and WiFi access point is ready
    When provisioning page is opened
    and valid credentials are entered
    Then provisioning should succeed

Not Connected to Firmware Access Point
    Given firmware is waiting for provisioning
    When provisioning page is opened without connecting to AP
    Then error should be indicated  ${MESSAGE_NOT_CONNECTED_ERROR}
# frontend error when not connected to firmware's WiFi AP

# frontend error when using invalid password

# TODO

# Provisioning can be retried after failure without rebooting the firmware or
# frontend

# Provisioning can be aborted at any stage using the back button
# What to do when we are already checking the provisioned credentials? It is
# not easy to recover from this state back to the initial state. If the
# credentials were good, then provisioning will not work any more after retry.
# Could disable the back button at this state, the only time it should really
# work is in the initial state where the user might have entered the
# provisioning page unintenionally and they want to back out. Technically, any
# time before the password is provided is OK.

# frontend doesn't allow using obviously incorrect passkey. This should not
# even reach the firmware.
#   - less than 8 characters
#   - more than 64 characters
#   - not ascii? This might be OK to leave, there could be some AP that allows
#     UTF-8 encoded unicode or something

# duplicate SSID are filtered. In the case of mesh networks, it doesn't really
# make sense to show all the APs when they have the same SSID. Just show the
# one with the highest RSSI.

# provision hidden SSID

# provision 32 byte raw hex passkey. This is not actually supported by the
# provisioning library. It always utf8 encodes the password

# frontend error when using invalid SSID. This is only possible after adding
# hidden SSID support.

# A friendly message appears when the firmware is already provisioned
# and connected. This should prevent any confusion in case the provisioning
# page is openend again after provisioning is already complete.

*** Keywords ***

WiFi access point is ready
    Execute manual step  Ensure you have a WiFi AP available to connect the firmware to

firmware is waiting for provisioning
    Execute manual step  Reset the firmware so that it does not have WiFi provisioned

provisioning page is opened without connecting to AP
    Execute manual step  Open the WiFi provisioning menu using the button in the top right of the frontend

provisioning page is opened
    Execute manual step  Connect the device running the frontend to the firmware's WiFi AP
    Execute manual step  Open the WiFi provisioning menu using the button in the top right of the frontend

valid credentials are entered
    Execute manual step  Enter the correct credentials

provisioning should succeed
    Execute manual step  Check if firmware has connected to the AP successfully

error should be indicated
    [Arguments]  ${error}
    Execute manual step  Check if the following error is shown: ${error}
