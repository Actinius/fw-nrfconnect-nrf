.. _pdn_sample:

nRF9160: PDN
############

.. contents::
   :local:
   :depth: 2

The PDN sample demonstrates how to create and configure a Packet Data Protocol (PDP) context, activate a Packet Data Network connection, and receive events on its state and connectivity using the :ref:`pdn_readme` library.

Requirements
************

The sample supports the following development kit:

.. table-from-rows:: /includes/sample_board_rows.txt
   :header: heading
   :rows: nrf9160dk_nrf9160_ns

.. include:: /includes/spm.txt

Overview
********

The sample first initializes the :ref:`nrfxlib:nrf_modem` and registers to the Packet Domain Events notifications (using the `AT+CGEREP=1 <AT+CGEREP set command_>`_ AT command) and notifications for unsolicited reporting of error codes sent by the network (using the `AT+CNEC=16 <AT+CNEC set command_>`_ AT command).
Next, the sample initializes the :ref:`pdn_readme` library and registers a callback for events pertaining to the default PDP context.
This is done before changing the function mode to 1 (``AT+CFUN=1``) to receive the activation event for the default PDP context.
The sample then creates a new PDP context and configures it to use the default APN, registers a callback for its events and activates the PDN connection.
Finally, the sample prints the PDP context IDs and PDN IDs of both the default PDP context and the new PDP context that it has created.


Building and running
********************

.. |sample path| replace:: :file:`samples/nrf9160/pdn`

.. include:: /includes/build_and_run_nrf9160.txt


Testing
=======

|test_sample|

1. |connect_kit|
#. |connect_terminal|
#. Power on or reset your nRF9160 DK.
#. Observe that the sample starts, creates and configures a PDP context, and then activates a PDN connection.

Sample Output
=============

The sample shows the following output, which may vary based on the network provider:

.. code-block:: console

   PDN sample started
   Event: PDP context 0 activated
   Created new PDP context 1
   Default APN is telenor.iot
   PDP context 1 configured: APN telenor.iot, Family IPV4V6
   Event: PDP context 0 IPv6 up
   Event: PDP context 1 activated
   Event: PDP context 1 IPv6 up
   PDP Context 0 (cid)      PDN ID 0
   PDP Context 1 (cid)      PDN ID 0
   Bye

Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`at_cmd_readme`
* :ref:`at_notif_readme`
* :ref:`lte_lc_readme`
* :ref:`pdn_readme`

It uses the following `sdk-nrfxlib`_ library:

* :ref:`nrfxlib:nrf_modem`

In addition, it uses the following sample:

* :ref:`secure_partition_manager`
