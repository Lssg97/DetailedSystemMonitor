unit adl_defines;

interface

//  Copyright (c) 2008 - 2013 Advanced Micro Devices, Inc.

//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
//  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

/// \file adl_defines.h
/// \brief Contains all definitions exposed by ADL for \ALL platforms.\n <b>Included in ADL SDK</b>
///
/// This file contains all definitions used by ADL.
/// The ADL definitions include the following:
/// \li ADL error codes
/// \li Enumerations for the ADLDisplayInfo structure
/// \li Maximum limits
///

/// \defgroup DEFINES Constants and Definitions
// @{

/// \defgroup define_misc Miscellaneous Constant Definitions
// @{

/// \name General Definitions
// @{

const

  /// Defines ADL_TRUE
  ADL_TRUE = 1;
  /// Defines ADL_FALSE
  ADL_FALSE = 0;

  /// Defines the maximum string length
  ADL_MAX_CHAR = 4096;
  /// Defines the maximum string length
  ADL_MAX_PATH = 256;
  /// Defines the maximum number of supported adapters
  ADL_MAX_ADAPTERS = 250;
  /// Defines the maxumum number of supported displays
  ADL_MAX_DISPLAYS = 150;
  /// Defines the maxumum string length for device name
  ADL_MAX_DEVICENAME = 32;
  /// Defines for all adapters
  ADL_ADAPTER_INDEX_ALL = -1;
  ///  Defines APIs with iOption none
  ADL_MAIN_API_OPTION_NONE = 0;
  // @}

  /// \name Definitions for iOption parameter used by
  /// ADL_Display_DDCBlockAccess_Get()
  // @{

  /// Switch to DDC line 2 before sending the command to the display.
  ADL_DDC_OPTION_SWITCHDDC2 = $00000001;
  /// Save command in the registry under a unique key, corresponding to parameter \b iCommandIndex
  ADL_DDC_OPTION_RESTORECOMMAND = $00000002;
  /// Combine write-read DDC block access command.
  ADL_DDC_OPTION_COMBOWRITEREAD = $00000010;
  /// Direct DDC access to the immediate device connected to graphics card.
  /// MST with this option set: DDC command is sent to first branch.
  /// MST with this option not set: DDC command is sent to the end node sink device.
  ADL_DDC_OPTION_SENDTOIMMEDIATEDEVICE = $00000020;
  // @}

  /// \name Values for
  /// ADLI2C.iAction used with ADL_Display_WriteAndReadI2C()
  // @{

  ADL_DL_I2C_ACTIONREAD = $00000001;
  ADL_DL_I2C_ACTIONWRITE = $00000002;
  ADL_DL_I2C_ACTIONREAD_REPEATEDSTART = $00000003;
  // @}

  // @}    //Misc

  /// \defgroup define_adl_results Result Codes
  /// This group of definitions are the various results returned by all ADL functions \n
  // @{
  /// All OK, but need to wait
  ADL_OK_WAIT = 4;
  /// All OK, but need restart
  ADL_OK_RESTART = 3;
  /// All OK but need mode change
  ADL_OK_MODE_CHANGE = 2;
  /// All OK, but with warning
  ADL_OK_WARNING = 1;
  /// ADL function completed successfully
  ADL_OK = 0;
  /// Generic Error. Most likely one or more of the Escape calls to the driver failed!
  ADL_ERR = -1;
  /// ADL not initialized
  ADL_ERR_NOT_INIT = -2;
  /// One of the parameter passed is invalid
  ADL_ERR_INVALID_PARAM = -3;
  /// One of the parameter size is invalid
  ADL_ERR_INVALID_PARAM_SIZE = -4;
  /// Invalid ADL index passed
  ADL_ERR_INVALID_ADL_IDX = -5;
  /// Invalid controller index passed
  ADL_ERR_INVALID_CONTROLLER_IDX = -6;
  /// Invalid display index passed
  ADL_ERR_INVALID_DIPLAY_IDX = -7;
  /// Function  not supported by the driver
  ADL_ERR_NOT_SUPPORTED = -8;
  /// Null Pointer error
  ADL_ERR_NULL_POINTER = -9;
  /// Call can't be made due to disabled adapter
  ADL_ERR_DISABLED_ADAPTER = -10;
  /// Invalid Callback
  ADL_ERR_INVALID_CALLBACK = -11;
  /// Display Resource conflict
  ADL_ERR_RESOURCE_CONFLICT = -12;
  //Failed to update some of the values. Can be returned by set request that include multiple values if not all values were successfully committed.
  ADL_ERR_SET_INCOMPLETE = -20;
  /// There's no Linux XDisplay in Linux Console environment
  ADL_ERR_NO_XDISPLAY = -21;

  // @}
  /// </A>

  /// \defgroup define_display_type Display Type
  /// Define Monitor/CRT display type
  // @{
  /// Define Monitor display type
  ADL_DT_MONITOR = 0;
  /// Define TV display type
  ADL_DT_TELEVISION = 1;
  /// Define LCD display type
  ADL_DT_LCD_PANEL = 2;
  /// Define DFP display type
  ADL_DT_DIGITAL_FLAT_PANEL = 3;
  /// Define Componment Video display type
  ADL_DT_COMPONENT_VIDEO = 4;
  /// Define Projector display type
  ADL_DT_PROJECTOR = 5;
  // @}

  /// \defgroup define_display_connection_type Display Connection Type
  // @{
  /// Define unknown display output type
  ADL_DOT_UNKNOWN = 0;
  /// Define composite display output type
  ADL_DOT_COMPOSITE = 1;
  /// Define SVideo display output type
  ADL_DOT_SVIDEO = 2;
  /// Define analog display output type
  ADL_DOT_ANALOG = 3;
  /// Define digital display output type
  ADL_DOT_DIGITAL = 4;
  // @}

  /// \defgroup define_color_type Display Color Type and Source
  /// Define  Display Color Type and Source
  // @{
  ADL_DISPLAY_COLOR_BRIGHTNESS = (1 shl 0);
  ADL_DISPLAY_COLOR_CONTRAST = (1 shl 1);
  ADL_DISPLAY_COLOR_SATURATION = (1 shl 2);
  ADL_DISPLAY_COLOR_HUE = (1 shl 3);
  ADL_DISPLAY_COLOR_TEMPERATURE = (1 shl 4);

  /// Color Temperature Source is EDID
  ADL_DISPLAY_COLOR_TEMPERATURE_SOURCE_EDID = (1 shl 5);
  /// Color Temperature Source is User
  ADL_DISPLAY_COLOR_TEMPERATURE_SOURCE_USER = (1 shl 6);
  // @}

  /// \defgroup define_adjustment_capabilities Display Adjustment Capabilities
  /// Display adjustment capabilities values.  Returned by ADL_Display_AdjustCaps_Get
  // @{
  ADL_DISPLAY_ADJUST_OVERSCAN = (1 shl 0);
  ADL_DISPLAY_ADJUST_VERT_POS = (1 shl 1);
  ADL_DISPLAY_ADJUST_HOR_POS = (1 shl 2);
  ADL_DISPLAY_ADJUST_VERT_SIZE = (1 shl 3);
  ADL_DISPLAY_ADJUST_HOR_SIZE = (1 shl 4);
  ADL_DISPLAY_ADJUST_SIZEPOS = (ADL_DISPLAY_ADJUST_VERT_POS or ADL_DISPLAY_ADJUST_HOR_POS or
    ADL_DISPLAY_ADJUST_VERT_SIZE or ADL_DISPLAY_ADJUST_HOR_SIZE);
  ADL_DISPLAY_CUSTOMMODES = (1 shl 5);
  ADL_DISPLAY_ADJUST_UNDERSCAN = (1 shl 6);
  // @}

  ///Down-scale support
  ADL_DISPLAY_CAPS_DOWNSCALE = (1 shl 0);

  /// Sharpness support
  ADL_DISPLAY_CAPS_SHARPNESS = (1 shl 0);

  /// \defgroup define_desktop_config Desktop Configuration Flags
  /// These flags are used by ADL_DesktopConfig_xxx
  /// \deprecated This API has been deprecated because it was only used for RandR 1.1 (Red Hat 5.x) distributions which is now not supported.
  // @{
  ADL_DESKTOPCONFIG_UNKNOWN = 0; //* UNKNOWN desktop config */;
  ADL_DESKTOPCONFIG_SINGLE = (1 shl 0); //* Single */;
  ADL_DESKTOPCONFIG_CLONE = (1 shl 2); //* Clone */;
  ADL_DESKTOPCONFIG_BIGDESK_H = (1 shl 4); //* Big Desktop Horizontal */;
  ADL_DESKTOPCONFIG_BIGDESK_V = (1 shl 5); //* Big Desktop Vertical */;
  ADL_DESKTOPCONFIG_BIGDESK_HR = (1 shl 6); //* Big Desktop Reverse Horz */;
  ADL_DESKTOPCONFIG_BIGDESK_VR = (1 shl 7); //* Big Desktop Reverse Vert */;
  ADL_DESKTOPCONFIG_RANDR12 = (1 shl 8); //* RandR 1.2 Multi-display */;
  // @}

  /// needed for ADLDDCInfo structure
  ADL_MAX_DISPLAY_NAME = 256;

  /// \defgroup define_edid_flags Values for ulDDCInfoFlag
  /// defines for ulDDCInfoFlag EDID flag
  // @{
  ADL_DISPLAYDDCINFOEX_FLAG_PROJECTORDEVICE = (1 shl 0);
  ADL_DISPLAYDDCINFOEX_FLAG_EDIDEXTENSION = (1 shl 1);
  ADL_DISPLAYDDCINFOEX_FLAG_DIGITALDEVICE = (1 shl 2);
  ADL_DISPLAYDDCINFOEX_FLAG_HDMIAUDIODEVICE = (1 shl 3);
  ADL_DISPLAYDDCINFOEX_FLAG_SUPPORTS_AI = (1 shl 4);
  ADL_DISPLAYDDCINFOEX_FLAG_SUPPORT_xvYCC601 = (1 shl 5);
  ADL_DISPLAYDDCINFOEX_FLAG_SUPPORT_xvYCC709 = (1 shl 6);
  // @}

  /// \defgroup define_displayinfo_connector Display Connector Type
  /// defines for ADLDisplayInfo.iDisplayConnector
  // @{
  ADL_DISPLAY_CONTYPE_UNKNOWN = 0;
  ADL_DISPLAY_CONTYPE_VGA = 1;
  ADL_DISPLAY_CONTYPE_DVI_D = 2;
  ADL_DISPLAY_CONTYPE_DVI_I = 3;
  ADL_DISPLAY_CONTYPE_ATICVDONGLE_NTSC = 4;
  ADL_DISPLAY_CONTYPE_ATICVDONGLE_JPN = 5;
  ADL_DISPLAY_CONTYPE_ATICVDONGLE_NONI2C_JPN = 6;
  ADL_DISPLAY_CONTYPE_ATICVDONGLE_NONI2C_NTSC = 7;
  ADL_DISPLAY_CONTYPE_PROPRIETARY = 8;
  ADL_DISPLAY_CONTYPE_HDMI_TYPE_A = 10;
  ADL_DISPLAY_CONTYPE_HDMI_TYPE_B = 11;
  ADL_DISPLAY_CONTYPE_SVIDEO = 12;
  ADL_DISPLAY_CONTYPE_COMPOSITE = 13;
  ADL_DISPLAY_CONTYPE_RCA_3COMPONENT = 14;
  ADL_DISPLAY_CONTYPE_DISPLAYPORT = 15;
  ADL_DISPLAY_CONTYPE_EDP = 16;
  ADL_DISPLAY_CONTYPE_WIRELESSDISPLAY = 17;
  // @}

  /// TV Capabilities and Standards
  /// \defgroup define_tv_caps TV Capabilities and Standards
  /// \deprecated Dropping support for TV displays
  // @{
  ADL_TV_STANDARDS = (1 shl 0);
  ADL_TV_SCART = (1 shl 1);

  /// TV Standards Definitions
  ADL_STANDARD_NTSC_M = (1 shl 0);
  ADL_STANDARD_NTSC_JPN = (1 shl 1);
  ADL_STANDARD_NTSC_N = (1 shl 2);
  ADL_STANDARD_PAL_B = (1 shl 3);
  ADL_STANDARD_PAL_COMB_N = (1 shl 4);
  ADL_STANDARD_PAL_D = (1 shl 5);
  ADL_STANDARD_PAL_G = (1 shl 6);
  ADL_STANDARD_PAL_H = (1 shl 7);
  ADL_STANDARD_PAL_I = (1 shl 8);
  ADL_STANDARD_PAL_K = (1 shl 9);
  ADL_STANDARD_PAL_K1 = (1 shl 10);
  ADL_STANDARD_PAL_L = (1 shl 11);
  ADL_STANDARD_PAL_M = (1 shl 12);
  ADL_STANDARD_PAL_N = (1 shl 13);
  ADL_STANDARD_PAL_SECAM_D = (1 shl 14);
  ADL_STANDARD_PAL_SECAM_K = (1 shl 15);
  ADL_STANDARD_PAL_SECAM_K1 = (1 shl 16);
  ADL_STANDARD_PAL_SECAM_L = (1 shl 17);
  // @}

  /// \defgroup define_video_custom_mode Video Custom Mode flags
  /// Component Video Custom Mode flags.  This is used by the iFlags parameter in ADLCustomMode
  // @{
  ADL_CUSTOMIZEDMODEFLAG_MODESUPPORTED = (1 shl 0);
  ADL_CUSTOMIZEDMODEFLAG_NOTDELETETABLE = (1 shl 1);
  ADL_CUSTOMIZEDMODEFLAG_INSERTBYDRIVER = (1 shl 2);
  ADL_CUSTOMIZEDMODEFLAG_INTERLACED = (1 shl 3);
  ADL_CUSTOMIZEDMODEFLAG_BASEMODE = (1 shl 4);
  // @}

  /// \defgroup define_ddcinfoflag Values used for DDCInfoFlag
  /// ulDDCInfoFlag field values used by the ADLDDCInfo structure
  // @{
{  ADL_DISPLAYDDCINFOEX_FLAG_PROJECTORDEVICE = (1 shl 0);
  ADL_DISPLAYDDCINFOEX_FLAG_EDIDEXTENSION = (1 shl 1);
  ADL_DISPLAYDDCINFOEX_FLAG_DIGITALDEVICE = (1 shl 2);
  ADL_DISPLAYDDCINFOEX_FLAG_HDMIAUDIODEVICE = (1 shl 3);
  ADL_DISPLAYDDCINFOEX_FLAG_SUPPORTS_AI = (1 shl 4);
  ADL_DISPLAYDDCINFOEX_FLAG_SUPPORT_xvYCC601 = (1 shl 5);
  ADL_DISPLAYDDCINFOEX_FLAG_SUPPORT_xvYCC709 = (1 shl 6);
}  // @}

  /// \defgroup define_cv_dongle Values used by ADL_CV_DongleSettings_xxx
  /// The following is applicable to ADL_DISPLAY_CONTYPE_ATICVDONGLE_JP and ADL_DISPLAY_CONTYPE_ATICVDONGLE_NONI2C_D only
  /// \deprecated Dropping support for Component Video displays
  // @{
  ADL_DISPLAY_CV_DONGLE_D1 = (1 shl 0);
  ADL_DISPLAY_CV_DONGLE_D2 = (1 shl 1);
  ADL_DISPLAY_CV_DONGLE_D3 = (1 shl 2);
  ADL_DISPLAY_CV_DONGLE_D4 = (1 shl 3);
  ADL_DISPLAY_CV_DONGLE_D5 = (1 shl 4);

  /// The following is applicable to ADL_DISPLAY_CONTYPE_ATICVDONGLE_NA and ADL_DISPLAY_CONTYPE_ATICVDONGLE_NONI2C only

  ADL_DISPLAY_CV_DONGLE_480I = (1 shl 0);
  ADL_DISPLAY_CV_DONGLE_480P = (1 shl 1);
  ADL_DISPLAY_CV_DONGLE_540P = (1 shl 2);
  ADL_DISPLAY_CV_DONGLE_720P = (1 shl 3);
  ADL_DISPLAY_CV_DONGLE_1080I = (1 shl 4);
  ADL_DISPLAY_CV_DONGLE_1080P = (1 shl 5);
  ADL_DISPLAY_CV_DONGLE_16_9 = (1 shl 6);
  ADL_DISPLAY_CV_DONGLE_720P50 = (1 shl 7);
  ADL_DISPLAY_CV_DONGLE_1080I25 = (1 shl 8);
  ADL_DISPLAY_CV_DONGLE_576I25 = (1 shl 9);
  ADL_DISPLAY_CV_DONGLE_576P50 = (1 shl 10);
  ADL_DISPLAY_CV_DONGLE_1080P24 = (1 shl 11);
  ADL_DISPLAY_CV_DONGLE_1080P25 = (1 shl 12);
  ADL_DISPLAY_CV_DONGLE_1080P30 = (1 shl 13);
  ADL_DISPLAY_CV_DONGLE_1080P50 = (1 shl 14);
  // @}

  /// \defgroup define_formats_ovr  Formats Override Settings
  /// Display force modes flags
  // @{
  ///
  ADL_DISPLAY_FORMAT_FORCE_720P = $00000001;
  ADL_DISPLAY_FORMAT_FORCE_1080I = $00000002;
  ADL_DISPLAY_FORMAT_FORCE_1080P = $00000004;
  ADL_DISPLAY_FORMAT_FORCE_720P50 = $00000008;
  ADL_DISPLAY_FORMAT_FORCE_1080I25 = $00000010;
  ADL_DISPLAY_FORMAT_FORCE_576I25 = $00000020;
  ADL_DISPLAY_FORMAT_FORCE_576P50 = $00000040;
  ADL_DISPLAY_FORMAT_FORCE_1080P24 = $00000080;
  ADL_DISPLAY_FORMAT_FORCE_1080P25 = $00000100;
  ADL_DISPLAY_FORMAT_FORCE_1080P30 = $00000200;
  ADL_DISPLAY_FORMAT_FORCE_1080P50 = $00000400;

  ///< Below are \b EXTENDED display mode flags

  ADL_DISPLAY_FORMAT_CVDONGLEOVERIDE = $00000001;
  ADL_DISPLAY_FORMAT_CVMODEUNDERSCAN = $00000002;
  ADL_DISPLAY_FORMAT_FORCECONNECT_SUPPORTED = $00000004;
  ADL_DISPLAY_FORMAT_RESTRICT_FORMAT_SELECTION = $00000008;
  ADL_DISPLAY_FORMAT_SETASPECRATIO = $00000010;
  ADL_DISPLAY_FORMAT_FORCEMODES = $00000020;
  ADL_DISPLAY_FORMAT_LCDRTCCOEFF = $00000040;
  // @}

  /// Defines used by OD5
  ADL_PM_PARAM_DONT_CHANGE = 0;

  /// The following defines Bus types
  // @{
  ADL_BUSTYPE_PCI = 0; //* PCI bus */;
  ADL_BUSTYPE_AGP = 1; //* AGP bus */;
  ADL_BUSTYPE_PCIE = 2; //* PCI Express bus */;
  ADL_BUSTYPE_PCIE_GEN2 = 3; //* PCI Express 2nd generation bus */;
  ADL_BUSTYPE_PCIE_GEN3 = 4; //* PCI Express 3rd generation bus */;
  // @}

  /// \defgroup define_ws_caps  Workstation Capabilities
  /// Workstation values
  // @{

  /// This value indicates that the workstation card supports active stereo though stereo output connector
  ADL_STEREO_SUPPORTED = (1 shl 2);
  /// This value indicates that the workstation card supports active stereo via "blue-line"
  ADL_STEREO_BLUE_LINE = (1 shl 3);
  /// This value is used to turn off stereo mode.
  ADL_STEREO_OFF = 0;
  /// This value indicates that the workstation card supports active stereo.  This is also used to set the stereo mode to active though the stereo output connector
  ADL_STEREO_ACTIVE = (1 shl 1);
  /// This value indicates that the workstation card supports auto-stereo monitors with horizontal interleave. This is also used to set the stereo mode to use the auto-stereo monitor with horizontal interleave
  ADL_STEREO_AUTO_HORIZONTAL = (1 shl 30);
  /// This value indicates that the workstation card supports auto-stereo monitors with vertical interleave. This is also used to set the stereo mode to use the auto-stereo monitor with vertical interleave
  ADL_STEREO_AUTO_VERTICAL = (1 shl 31);
  /// This value indicates that the workstation card supports passive stereo, ie. non stereo sync
  ADL_STEREO_PASSIVE = (1 shl 6);
  /// This value indicates that the workstation card supports auto-stereo monitors with vertical interleave. This is also used to set the stereo mode to use the auto-stereo monitor with vertical interleave
  ADL_STEREO_PASSIVE_HORIZ = (1 shl 7);
  /// This value indicates that the workstation card supports auto-stereo monitors with vertical interleave. This is also used to set the stereo mode to use the auto-stereo monitor with vertical interleave
  ADL_STEREO_PASSIVE_VERT = (1 shl 8);
  /// This value indicates that the workstation card supports auto-stereo monitors with Samsung.
  ADL_STEREO_AUTO_SAMSUNG = (1 shl 11);
  /// This value indicates that the workstation card supports auto-stereo monitors with Tridility.
  ADL_STEREO_AUTO_TSL = (1 shl 12);
  /// This value indicates that the workstation card supports DeepBitDepth (10 bpp)
  ADL_DEEPBITDEPTH_10BPP_SUPPORTED = (1 shl 5);

  /// This value indicates that the workstation supports 8-Bit Grayscale
  ADL_8BIT_GREYSCALE_SUPPORTED = (1 shl 9);
  /// This value indicates that the workstation supports CUSTOM TIMING
  ADL_CUSTOM_TIMING_SUPPORTED = (1 shl 10);

  /// Load balancing is supported.
  ADL_WORKSTATION_LOADBALANCING_SUPPORTED = $00000001;
  /// Load balancing is available.
  ADL_WORKSTATION_LOADBALANCING_AVAILABLE = $00000002;

  /// Load balancing is disabled.
  ADL_WORKSTATION_LOADBALANCING_DISABLED = $00000000;
  /// Load balancing is Enabled.
  ADL_WORKSTATION_LOADBALANCING_ENABLED = $00000001;

  // @}

  /// \defgroup define_adapterspeed speed setting from the adapter
  // @{
  ADL_CONTEXT_SPEED_UNFORCED = 0; //* default asic running speed */;
  ADL_CONTEXT_SPEED_FORCEHIGH = 1; //* asic running speed is forced to high */;
  ADL_CONTEXT_SPEED_FORCELOW = 2; //* asic running speed is forced to low */;

  ADL_ADAPTER_SPEEDCAPS_SUPPORTED = (1 shl 0); //* change asic running speed setting is supported */;
  // @}

  /// \defgroup define_glsync Genlock related values
  /// GL-Sync port types (unique values)
  // @{
  /// Unknown port of GL-Sync module
  ADL_GLSYNC_PORT_UNKNOWN = 0;
  /// BNC port of of GL-Sync module
  ADL_GLSYNC_PORT_BNC = 1;
  /// RJ45(1) port of of GL-Sync module
  ADL_GLSYNC_PORT_RJ45PORT1 = 2;
  /// RJ45(2) port of of GL-Sync module
  ADL_GLSYNC_PORT_RJ45PORT2 = 3;

  // GL-Sync Genlock settings mask (bit-vector)

  /// None of the ADLGLSyncGenlockConfig members are valid
  ADL_GLSYNC_CONFIGMASK_NONE = 0;
  /// The ADLGLSyncGenlockConfig.lSignalSource member is valid
  ADL_GLSYNC_CONFIGMASK_SIGNALSOURCE = (1 shl 0);
  /// The ADLGLSyncGenlockConfig.iSyncField member is valid
  ADL_GLSYNC_CONFIGMASK_SYNCFIELD = (1 shl 1);
  /// The ADLGLSyncGenlockConfig.iSampleRate member is valid
  ADL_GLSYNC_CONFIGMASK_SAMPLERATE = (1 shl 2);
  /// The ADLGLSyncGenlockConfig.lSyncDelay member is valid
  ADL_GLSYNC_CONFIGMASK_SYNCDELAY = (1 shl 3);
  /// The ADLGLSyncGenlockConfig.iTriggerEdge member is valid
  ADL_GLSYNC_CONFIGMASK_TRIGGEREDGE = (1 shl 4);
  /// The ADLGLSyncGenlockConfig.iScanRateCoeff member is valid
  ADL_GLSYNC_CONFIGMASK_SCANRATECOEFF = (1 shl 5);
  /// The ADLGLSyncGenlockConfig.lFramelockCntlVector member is valid
  ADL_GLSYNC_CONFIGMASK_FRAMELOCKCNTL = (1 shl 6);

  // GL-Sync Framelock control mask (bit-vector)

  /// Framelock is disabled
  ADL_GLSYNC_FRAMELOCKCNTL_NONE = 0;
  /// Framelock is enabled
  ADL_GLSYNC_FRAMELOCKCNTL_ENABLE = (1 shl 0);

  ADL_GLSYNC_FRAMELOCKCNTL_DISABLE = (1 shl 1);
  ADL_GLSYNC_FRAMELOCKCNTL_SWAP_COUNTER_RESET = (1 shl 2);
  ADL_GLSYNC_FRAMELOCKCNTL_SWAP_COUNTER_ACK = (1 shl 3);
  ADL_GLSYNC_FRAMELOCKCNTL_VERSION_KMD = (1 shl 4);

  ADL_GLSYNC_FRAMELOCKCNTL_STATE_ENABLE = (1 shl 0);
  ADL_GLSYNC_FRAMELOCKCNTL_STATE_KMD = (1 shl 4);

  // GL-Sync Framelock counters mask (bit-vector)
  ADL_GLSYNC_COUNTER_SWAP = (1 shl 0);

  // GL-Sync Signal Sources (unique values)

  /// GL-Sync signal source is undefined
  ADL_GLSYNC_SIGNALSOURCE_UNDEFINED = $00000100;
  /// GL-Sync signal source is Free Run
  ADL_GLSYNC_SIGNALSOURCE_FREERUN = $00000101;
  /// GL-Sync signal source is the BNC GL-Sync port
  ADL_GLSYNC_SIGNALSOURCE_BNCPORT = $00000102;
  /// GL-Sync signal source is the RJ45(1) GL-Sync port
  ADL_GLSYNC_SIGNALSOURCE_RJ45PORT1 = $00000103;
  /// GL-Sync signal source is the RJ45(2) GL-Sync port
  ADL_GLSYNC_SIGNALSOURCE_RJ45PORT2 = $00000104;

  // GL-Sync Signal Types (unique values)

  /// GL-Sync signal type is unknown
  ADL_GLSYNC_SIGNALTYPE_UNDEFINED = 0;
  /// GL-Sync signal type is 480I
  ADL_GLSYNC_SIGNALTYPE_480I = 1;
  /// GL-Sync signal type is 576I
  ADL_GLSYNC_SIGNALTYPE_576I = 2;
  /// GL-Sync signal type is 480P
  ADL_GLSYNC_SIGNALTYPE_480P = 3;
  /// GL-Sync signal type is 576P
  ADL_GLSYNC_SIGNALTYPE_576P = 4;
  /// GL-Sync signal type is 720P
  ADL_GLSYNC_SIGNALTYPE_720P = 5;
  /// GL-Sync signal type is 1080P
  ADL_GLSYNC_SIGNALTYPE_1080P = 6;
  /// GL-Sync signal type is 1080I
  ADL_GLSYNC_SIGNALTYPE_1080I = 7;
  /// GL-Sync signal type is SDI
  ADL_GLSYNC_SIGNALTYPE_SDI = 8;
  /// GL-Sync signal type is TTL
  ADL_GLSYNC_SIGNALTYPE_TTL = 9;
  /// GL_Sync signal type is Analog
  ADL_GLSYNC_SIGNALTYPE_ANALOG = 10;

  // GL-Sync Sync Field options (unique values)

  ///GL-Sync sync field option is undefined
  ADL_GLSYNC_SYNCFIELD_UNDEFINED = 0;
  ///GL-Sync sync field option is Sync to Field 1 (used for Interlaced signal types)
  ADL_GLSYNC_SYNCFIELD_BOTH = 1;
  ///GL-Sync sync field option is Sync to Both fields (used for Interlaced signal types)
  ADL_GLSYNC_SYNCFIELD_1 = 2;

  // GL-Sync trigger edge options (unique values)

  /// GL-Sync trigger edge is undefined
  ADL_GLSYNC_TRIGGEREDGE_UNDEFINED = 0;
  /// GL-Sync trigger edge is the rising edge
  ADL_GLSYNC_TRIGGEREDGE_RISING = 1;
  /// GL-Sync trigger edge is the falling edge
  ADL_GLSYNC_TRIGGEREDGE_FALLING = 2;
  /// GL-Sync trigger edge is both the rising and the falling edge
  ADL_GLSYNC_TRIGGEREDGE_BOTH = 3;

  // GL-Sync scan rate coefficient/multiplier options (unique values)

  /// GL-Sync scan rate coefficient/multiplier is undefined
  ADL_GLSYNC_SCANRATECOEFF_UNDEFINED = 0;
  /// GL-Sync scan rate coefficient/multiplier is 5
  ADL_GLSYNC_SCANRATECOEFF_x5 = 1;
  /// GL-Sync scan rate coefficient/multiplier is 4
  ADL_GLSYNC_SCANRATECOEFF_x4 = 2;
  /// GL-Sync scan rate coefficient/multiplier is 3
  ADL_GLSYNC_SCANRATECOEFF_x3 = 3;
  /// GL-Sync scan rate coefficient/multiplier is 5:2 (SMPTE)
  ADL_GLSYNC_SCANRATECOEFF_x5_DIV_2 = 4;
  /// GL-Sync scan rate coefficient/multiplier is 2
  ADL_GLSYNC_SCANRATECOEFF_x2 = 5;
  /// GL-Sync scan rate coefficient/multiplier is 3 : 2
  ADL_GLSYNC_SCANRATECOEFF_x3_DIV_2 = 6;
  /// GL-Sync scan rate coefficient/multiplier is 5 : 4
  ADL_GLSYNC_SCANRATECOEFF_x5_DIV_4 = 7;
  /// GL-Sync scan rate coefficient/multiplier is 1 (default)
  ADL_GLSYNC_SCANRATECOEFF_x1 = 8;
  /// GL-Sync scan rate coefficient/multiplier is 4 : 5
  ADL_GLSYNC_SCANRATECOEFF_x4_DIV_5 = 9;
  /// GL-Sync scan rate coefficient/multiplier is 2 : 3
  ADL_GLSYNC_SCANRATECOEFF_x2_DIV_3 = 10;
  /// GL-Sync scan rate coefficient/multiplier is 1 : 2
  ADL_GLSYNC_SCANRATECOEFF_x1_DIV_2 = 11;
  /// GL-Sync scan rate coefficient/multiplier is 2 : 5 (SMPTE)
  ADL_GLSYNC_SCANRATECOEFF_x2_DIV_5 = 12;
  /// GL-Sync scan rate coefficient/multiplier is 1 : 3
  ADL_GLSYNC_SCANRATECOEFF_x1_DIV_3 = 13;
  /// GL-Sync scan rate coefficient/multiplier is 1 : 4
  ADL_GLSYNC_SCANRATECOEFF_x1_DIV_4 = 14;
  /// GL-Sync scan rate coefficient/multiplier is 1 : 5
  ADL_GLSYNC_SCANRATECOEFF_x1_DIV_5 = 15;

  // GL-Sync port (signal presence) states (unique values)

  /// GL-Sync port state is undefined
  ADL_GLSYNC_PORTSTATE_UNDEFINED = 0;
  /// GL-Sync port is not connected
  ADL_GLSYNC_PORTSTATE_NOCABLE = 1;
  /// GL-Sync port is Idle
  ADL_GLSYNC_PORTSTATE_IDLE = 2;
  /// GL-Sync port has an Input signal
  ADL_GLSYNC_PORTSTATE_INPUT = 3;
  /// GL-Sync port is Output
  ADL_GLSYNC_PORTSTATE_OUTPUT = 4;

  // GL-Sync LED types (used index within ADL_Workstation_GLSyncPortState_Get returned ppGlSyncLEDs array) (unique values)

  /// Index into the ADL_Workstation_GLSyncPortState_Get returned ppGlSyncLEDs array for the one LED of the BNC port
  ADL_GLSYNC_LEDTYPE_BNC = 0;
  /// Index into the ADL_Workstation_GLSyncPortState_Get returned ppGlSyncLEDs array for the Left LED of the RJ45(1) or RJ45(2) port
  ADL_GLSYNC_LEDTYPE_RJ45_LEFT = 0;
  /// Index into the ADL_Workstation_GLSyncPortState_Get returned ppGlSyncLEDs array for the Right LED of the RJ45(1) or RJ45(2) port
  ADL_GLSYNC_LEDTYPE_RJ45_RIGHT = 1;

  // GL-Sync LED colors (unique values)

  /// GL-Sync LED undefined color
  ADL_GLSYNC_LEDCOLOR_UNDEFINED = 0;
  /// GL-Sync LED is unlit
  ADL_GLSYNC_LEDCOLOR_NOLIGHT = 1;
  /// GL-Sync LED is yellow
  ADL_GLSYNC_LEDCOLOR_YELLOW = 2;
  /// GL-Sync LED is red
  ADL_GLSYNC_LEDCOLOR_RED = 3;
  /// GL-Sync LED is green
  ADL_GLSYNC_LEDCOLOR_GREEN = 4;
  /// GL-Sync LED is flashing green
  ADL_GLSYNC_LEDCOLOR_FLASH_GREEN = 5;

  // GL-Sync Port Control (refers one GL-Sync Port) (unique values)

  /// Used to configure the RJ54(1) or RJ42(2) port of GL-Sync is as Idle
  ADL_GLSYNC_PORTCNTL_NONE = $00000000;
  /// Used to configure the RJ54(1) or RJ42(2) port of GL-Sync is as Output
  ADL_GLSYNC_PORTCNTL_OUTPUT = $00000001;

  // GL-Sync Mode Control (refers one Display/Controller) (bitfields)

  /// Used to configure the display to use internal timing (not genlocked)
  ADL_GLSYNC_MODECNTL_NONE = $00000000;
  /// Bitfield used to configure the display as genlocked (either as Timing Client or as Timing Server)
  ADL_GLSYNC_MODECNTL_GENLOCK = $00000001;
  /// Bitfield used to configure the display as Timing Server
  ADL_GLSYNC_MODECNTL_TIMINGSERVER = $00000002;

  // GL-Sync Mode Status
  /// Display is currently not genlocked
  ADL_GLSYNC_MODECNTL_STATUS_NONE = $00000000;
  /// Display is currently genlocked
  ADL_GLSYNC_MODECNTL_STATUS_GENLOCK = $00000001;
  /// Display requires a mode switch
  ADL_GLSYNC_MODECNTL_STATUS_SETMODE_REQUIRED = $00000002;
  /// Display is capable of being genlocked
  ADL_GLSYNC_MODECNTL_STATUS_GENLOCK_ALLOWED = $00000004;

  ADL_MAX_GLSYNC_PORTS = 8;
  ADL_MAX_GLSYNC_PORT_LEDS = 8;

  // @}

  /// \defgroup define_crossfirestate CrossfireX state of a particular adapter CrossfireX combination
  // @{
  ADL_XFIREX_STATE_NOINTERCONNECT = (1 shl 0); //* Dongle / cable is missing */;
  ADL_XFIREX_STATE_DOWNGRADEPIPES = (1 shl 1); //* CrossfireX can be enabled if pipes are downgraded */;
  ADL_XFIREX_STATE_DOWNGRADEMEM = (1 shl 2); //* CrossfireX cannot be enabled unless mem downgraded */;
  ADL_XFIREX_STATE_REVERSERECOMMENDED = (1 shl 3); //* Card reversal recommended, CrossfireX cannot be enabled. */;
  ADL_XFIREX_STATE_3DACTIVE = (1 shl 4); //* 3D client is active - CrossfireX cannot be safely enabled */;
  ADL_XFIREX_STATE_MASTERONSLAVE = (1 shl 5); //* Dongle is OK but master is on slave */;
  ADL_XFIREX_STATE_NODISPLAYCONNECT = (1 shl 6); //* No (valid) display connected to master card. */;
  ADL_XFIREX_STATE_NOPRIMARYVIEW = (1 shl 7); //* CrossfireX is enabled but master is not current primary device */;
  ADL_XFIREX_STATE_DOWNGRADEVISMEM = (1 shl 8); //* CrossfireX cannot be enabled unless visible mem downgraded */;
  ADL_XFIREX_STATE_LESSTHAN8LANE_MASTER = (1 shl 9);
  //* CrossfireX can be enabled however performance not optimal due to <8 lanes */;
  ADL_XFIREX_STATE_LESSTHAN8LANE_SLAVE = (1 shl 10);
  //* CrossfireX can be enabled however performance not optimal due to <8 lanes */;
  ADL_XFIREX_STATE_PEERTOPEERFAILED = (1 shl 11);
  //* CrossfireX cannot be enabled due to failed peer to peer test */;
  ADL_XFIREX_STATE_MEMISDOWNGRADED = (1 shl 16); //* Notification that memory is currently downgraded */;
  ADL_XFIREX_STATE_PIPESDOWNGRADED = (1 shl 17); //* Notification that pipes are currently downgraded */;
  ADL_XFIREX_STATE_XFIREXACTIVE = (1 shl 18); //* CrossfireX is enabled on current device */;
  ADL_XFIREX_STATE_VISMEMISDOWNGRADED = (1 shl 19);
  //* Notification that visible FB memory is currently downgraded */;
  ADL_XFIREX_STATE_INVALIDINTERCONNECTION = (1 shl 20); //* Cannot support current inter-connection configuration */;
  ADL_XFIREX_STATE_NONP2PMODE = (1 shl 21); //* CrossfireX will only work with clients supporting non P2P mode */;
  ADL_XFIREX_STATE_DOWNGRADEMEMBANKS = (1 shl 22);
  //* CrossfireX cannot be enabled unless memory banks downgraded */;
  ADL_XFIREX_STATE_MEMBANKSDOWNGRADED = (1 shl 23); //* Notification that memory banks are currently downgraded */;
  ADL_XFIREX_STATE_DUALDISPLAYSALLOWED = (1 shl 24); //* Extended desktop or clone mode is allowed. */;
  ADL_XFIREX_STATE_P2P_APERTURE_MAPPING = (1 shl 25); //* P2P mapping was through peer aperture */;
  ADL_XFIREX_STATE_P2PFLUSH_REQUIRED = ADL_XFIREX_STATE_P2P_APERTURE_MAPPING; //* For back compatible */;
  ADL_XFIREX_STATE_XSP_CONNECTED = (1 shl 26); //* There is CrossfireX side port connection between GPUs */;
  ADL_XFIREX_STATE_ENABLE_CF_REBOOT_REQUIRED = (1 shl 27); //* System needs a reboot bofore enable CrossfireX */;
  ADL_XFIREX_STATE_DISABLE_CF_REBOOT_REQUIRED = (1 shl 28); //* System needs a reboot after disable CrossfireX */;
  ADL_XFIREX_STATE_DRV_HANDLE_DOWNGRADE_KEY = (1 shl 29);
  //* Indicate base driver handles the downgrade key updating */;
  ADL_XFIREX_STATE_CF_RECONFIG_REQUIRED = (1 shl 30);
  //* CrossfireX need to be reconfigured by CCC because of a LDA chain broken */;
  ADL_XFIREX_STATE_ERRORGETTINGSTATUS = (1 shl 31); //* Could not obtain current status */;
  // @}

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_ADJUSTMENT_PIXELFORMAT adjustment values
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  /// \defgroup define_pixel_formats Pixel Formats values
  /// This group defines the various Pixel Formats that a particular digital display can support. \n
  /// Since a display can support multiple formats, these values can be bit-or'ed to indicate the various formats \n
  // @{
  ADL_DISPLAY_PIXELFORMAT_UNKNOWN = 0;
  ADL_DISPLAY_PIXELFORMAT_RGB = (1 shl 0);
  ADL_DISPLAY_PIXELFORMAT_YCRCB444 = (1 shl 1); //Limited range;
  ADL_DISPLAY_PIXELFORMAT_YCRCB422 = (1 shl 2); //Limited range;
  ADL_DISPLAY_PIXELFORMAT_RGB_LIMITED_RANGE = (1 shl 3);
  ADL_DISPLAY_PIXELFORMAT_RGB_FULL_RANGE = ADL_DISPLAY_PIXELFORMAT_RGB; //Full range;
  ADL_DISPLAY_PIXELFORMAT_YCRCB420 = (1 shl 4);
  // @}

  /// \defgroup define_contype Connector Type Values
  /// ADLDisplayConfig.ulConnectorType defines
  // @{
  ADL_DL_DISPLAYCONFIG_CONTYPE_UNKNOWN = 0;
  ADL_DL_DISPLAYCONFIG_CONTYPE_CV_NONI2C_JP = 1;
  ADL_DL_DISPLAYCONFIG_CONTYPE_CV_JPN = 2;
  ADL_DL_DISPLAYCONFIG_CONTYPE_CV_NA = 3;
  ADL_DL_DISPLAYCONFIG_CONTYPE_CV_NONI2C_NA = 4;
  ADL_DL_DISPLAYCONFIG_CONTYPE_VGA = 5;
  ADL_DL_DISPLAYCONFIG_CONTYPE_DVI_D = 6;
  ADL_DL_DISPLAYCONFIG_CONTYPE_DVI_I = 7;
  ADL_DL_DISPLAYCONFIG_CONTYPE_HDMI_TYPE_A = 8;
  ADL_DL_DISPLAYCONFIG_CONTYPE_HDMI_TYPE_B = 9;
  ADL_DL_DISPLAYCONFIG_CONTYPE_DISPLAYPORT = 10;
  // @}

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_DISPLAYINFO_ Definitions
  // for ADLDisplayInfo.iDisplayInfoMask and ADLDisplayInfo.iDisplayInfoValue
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  /// \defgroup define_displayinfomask Display Info Mask Values
  // @{
  ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED = $00000001;
  ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED = $00000002;
  ADL_DISPLAY_DISPLAYINFO_NONLOCAL = $00000004;
  ADL_DISPLAY_DISPLAYINFO_FORCIBLESUPPORTED = $00000008;
  ADL_DISPLAY_DISPLAYINFO_GENLOCKSUPPORTED = $00000010;
  ADL_DISPLAY_DISPLAYINFO_MULTIVPU_SUPPORTED = $00000020;
  ADL_DISPLAY_DISPLAYINFO_LDA_DISPLAY = $00000040;
  ADL_DISPLAY_DISPLAYINFO_MODETIMING_OVERRIDESSUPPORTED = $00000080;

  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_SINGLE = $00000100;
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_CLONE = $00000200;

  /// Legacy support for XP
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_2VSTRETCH = $00000400;
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_2HSTRETCH = $00000800;
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_EXTENDED = $00001000;

  /// More support manners
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_NSTRETCH1GPU = $00010000;
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_NSTRETCHNGPU = $00020000;
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_RESERVED2 = $00040000;
  ADL_DISPLAY_DISPLAYINFO_MANNER_SUPPORTED_RESERVED3 = $00080000;

  /// Projector display type
  ADL_DISPLAY_DISPLAYINFO_SHOWTYPE_PROJECTOR = $00100000;

  // @}

  ///////////////////////////////////////////////////////////////////////////
  // ADL_ADAPTER_DISPLAY_MANNER_SUPPORTED_ Definitions
  // for ADLAdapterDisplayCap of ADL_Adapter_Display_Cap()
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  /// \defgroup define_adaptermanner Adapter Manner Support Values
  // @{
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_NOTACTIVE = $00000001;
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_SINGLE = $00000002;
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_CLONE = $00000004;
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_NSTRETCH1GPU = $00000008;
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_NSTRETCHNGPU = $00000010;

  /// Legacy support for XP
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_2VSTRETCH = $00000020;
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_2HSTRETCH = $00000040;
  ADL_ADAPTER_DISPLAYCAP_MANNER_SUPPORTED_EXTENDED = $00000080;

  ADL_ADAPTER_DISPLAYCAP_PREFERDISPLAY_SUPPORTED = $00000100;
  ADL_ADAPTER_DISPLAYCAP_BEZEL_SUPPORTED = $00000200;

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_DISPLAYMAP_MANNER_ Definitions
  // for ADLDisplayMap.iDisplayMapMask and ADLDisplayMap.iDisplayMapValue
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  ADL_DISPLAY_DISPLAYMAP_MANNER_RESERVED = $00000001;
  ADL_DISPLAY_DISPLAYMAP_MANNER_NOTACTIVE = $00000002;
  ADL_DISPLAY_DISPLAYMAP_MANNER_SINGLE = $00000004;
  ADL_DISPLAY_DISPLAYMAP_MANNER_CLONE = $00000008;
  ADL_DISPLAY_DISPLAYMAP_MANNER_RESERVED1 = $00000010; // Removed NSTRETCH;
  ADL_DISPLAY_DISPLAYMAP_MANNER_HSTRETCH = $00000020;
  ADL_DISPLAY_DISPLAYMAP_MANNER_VSTRETCH = $00000040;
  ADL_DISPLAY_DISPLAYMAP_MANNER_VLD = $00000080;

  // @}

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_DISPLAYMAP_OPTION_ Definitions
  // for iOption in function ADL_Display_DisplayMapConfig_Get
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  ADL_DISPLAY_DISPLAYMAP_OPTION_GPUINFO = $00000001;

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_DISPLAYTARGET_ Definitions
  // for ADLDisplayTarget.iDisplayTargetMask and ADLDisplayTarget.iDisplayTargetValue
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  ADL_DISPLAY_DISPLAYTARGET_PREFERRED = $00000001;

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_POSSIBLEMAPRESULT_VALID Definitions
  // for ADLPossibleMapResult.iPossibleMapResultMask and ADLPossibleMapResult.iPossibleMapResultValue
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  ADL_DISPLAY_POSSIBLEMAPRESULT_VALID = $00000001;
  ADL_DISPLAY_POSSIBLEMAPRESULT_BEZELSUPPORTED = $00000002;
  ADL_DISPLAY_POSSIBLEMAPRESULT_OVERLAPSUPPORTED = $00000004;

  ///////////////////////////////////////////////////////////////////////////
  // ADL_DISPLAY_MODE_ Definitions
  // for ADLMode.iModeMask, ADLMode.iModeValue, and ADLMode.iModeFlag
  // (bit-vector)
  ///////////////////////////////////////////////////////////////////////////
  /// \defgroup define_displaymode Display Mode Values
  // @{
  ADL_DISPLAY_MODE_COLOURFORMAT_565 = $00000001;
  ADL_DISPLAY_MODE_COLOURFORMAT_8888 = $00000002;
  ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_000 = $00000004;
  ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_090 = $00000008;
  ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_180 = $00000010;
  ADL_DISPLAY_MODE_ORIENTATION_SUPPORTED_270 = $00000020;
  ADL_DISPLAY_MODE_REFRESHRATE_ROUNDED = $00000040;
  ADL_DISPLAY_MODE_REFRESHRATE_ONLY = $00000080;

  ADL_DISPLAY_MODE_PROGRESSIVE_FLAG = 0;
  ADL_DISPLAY_MODE_INTERLACED_FLAG = 2;
  // @}

  ///////////////////////////////////////////////////////////////////////////
  // ADL_OSMODEINFO Definitions
  ///////////////////////////////////////////////////////////////////////////
  /// \defgroup define_osmode OS Mode Values
  // @{
  ADL_OSMODEINFOXPOS_DEFAULT = -640;
  ADL_OSMODEINFOYPOS_DEFAULT = 0;
  ADL_OSMODEINFOXRES_DEFAULT = 640;
  ADL_OSMODEINFOYRES_DEFAULT = 480;
  ADL_OSMODEINFOXRES_DEFAULT800 = 800;
  ADL_OSMODEINFOYRES_DEFAULT600 = 600;
  ADL_OSMODEINFOREFRESHRATE_DEFAULT = 60;
  ADL_OSMODEINFOCOLOURDEPTH_DEFAULT = 8;
  ADL_OSMODEINFOCOLOURDEPTH_DEFAULT16 = 16;
  ADL_OSMODEINFOCOLOURDEPTH_DEFAULT24 = 24;
  ADL_OSMODEINFOCOLOURDEPTH_DEFAULT32 = 32;
  ADL_OSMODEINFOORIENTATION_DEFAULT = 0;
  ADL_OSMODEINFOORIENTATION_DEFAULT_WIN7 = $FFFFFFFF;
  ADL_OSMODEFLAG_DEFAULT = 0;
// @}

///////////////////////////////////////////////////////////////////////////
// ADLThreadingModel Enumeration
///////////////////////////////////////////////////////////////////////////
/// \defgroup thread_model
/// Used with \ref ADL_Main_ControlX2_Create and \ref ADL2_Main_ControlX2_Create to specify how ADL handles API calls when executed by multiple threads concurrently.
/// \brief Declares ADL threading behavior.
// @{
type
  ADLThreadingModel = (
    ADL_THREADING_UNLOCKED = 0,
    //; //*!< Default behavior. ADL will not enforce serialization of ADL API executions by multiple threads. Multiple threads will be allowed to enter to ADL at the same time. Note that ADL library is not guaranteed to be thread-safe. Client that calls ADL_Main_Control_Create have to provide its own mechanism for ADL calls serialization. */;
    ADL_THREADING_LOCKED);
//*!< ADL will enforce serialization of ADL API when called by multiple threads. Only single thread will be allowed to enter ADL API at the time. This option makes ADL calls thread-safe. You shouldn't use this option if ADL calls will be executed on Linux on x-server rendering thread. It can cause the application to hung. */;

// @}
///////////////////////////////////////////////////////////////////////////
// ADLPurposeCode Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLPurposeCode = (
    ADL_PURPOSECODE_NORMAL = 0,
    ADL_PURPOSECODE_HIDE_MODE_SWITCH,
    ADL_PURPOSECODE_MODE_SWITCH,
    ADL_PURPOSECODE_ATTATCH_DEVICE,
    ADL_PURPOSECODE_DETACH_DEVICE,
    ADL_PURPOSECODE_SETPRIMARY_DEVICE,
    ADL_PURPOSECODE_GDI_ROTATION,
    ADL_PURPOSECODE_ATI_ROTATION);
///////////////////////////////////////////////////////////////////////////
// ADLAngle Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLAngle = (
    ADL_ANGLE_LANDSCAPE = 0,
    ADL_ANGLE_ROTATERIGHT = 90,
    ADL_ANGLE_ROTATE180 = 180,
    ADL_ANGLE_ROTATELEFT = 270);

///////////////////////////////////////////////////////////////////////////
// ADLOrientationDataType Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLOrientationDataType = (
    ADL_ORIENTATIONTYPE_OSDATATYPE,
    ADL_ORIENTATIONTYPE_NONOSDATATYPE);

///////////////////////////////////////////////////////////////////////////
// ADLPanningMode Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLPanningMode = (
    ADL_PANNINGMODE_NO_PANNING = 0,
    ADL_PANNINGMODE_AT_LEAST_ONE_NO_PANNING = 1,
    ADL_PANNINGMODE_ALLOW_PANNING = 2);

///////////////////////////////////////////////////////////////////////////
// ADLLARGEDESKTOPTYPE Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLLARGEDESKTOPTYPE = (
    ADL_LARGEDESKTOPTYPE_NORMALDESKTOP = 0,
    ADL_LARGEDESKTOPTYPE_PSEUDOLARGEDESKTOP = 1,
    ADL_LARGEDESKTOPTYPE_VERYLARGEDESKTOP = 2);

///////////////////////////////////////////////////////////////////////////
// ADLPlatform Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLPlatForm = (
    GRAPHICS_PLATFORM_DESKTOP = 0,
    GRAPHICS_PLATFORM_MOBILE = 1);

///////////////////////////////////////////////////////////////////////////
// ADLGraphicCoreGeneration Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLGraphicCoreGeneration = (
    ADL_GRAPHIC_CORE_GENERATION_UNDEFINED = 0,
    ADL_GRAPHIC_CORE_GENERATION_PRE_GCN = 1,
    ADL_GRAPHIC_CORE_GENERATION_GCN = 2);

// Other Definitions for internal use

// Values for ADL_Display_WriteAndReadI2CRev_Get()

const

  ADL_I2C_MAJOR_API_REV = $00000001;
  ADL_I2C_MINOR_DEFAULT_API_REV = $00000000;
  ADL_I2C_MINOR_OEM_API_REV = $00000001;

  // Values for ADL_Display_WriteAndReadI2C()
  ADL_DL_I2C_LINE_OEM = $00000001;
  ADL_DL_I2C_LINE_OD_CONTROL = $00000002;
  ADL_DL_I2C_LINE_OEM2 = $00000003;
  ADL_DL_I2C_LINE_OEM3 = $00000004;
  ADL_DL_I2C_LINE_OEM4 = $00000005;
  ADL_DL_I2C_LINE_OEM5 = $00000006;
  ADL_DL_I2C_LINE_OEM6 = $00000007;

  // Max size of I2C data buffer
  ADL_DL_I2C_MAXDATASIZE = $00000040;
  ADL_DL_I2C_MAXWRITEDATASIZE = $0000000C;
  ADL_DL_I2C_MAXADDRESSLENGTH = $00000006;
  ADL_DL_I2C_MAXOFFSETLENGTH = $00000004;

  /// Values for ADLDisplayProperty.iPropertyType
  ADL_DL_DISPLAYPROPERTY_TYPE_UNKNOWN = 0;
  ADL_DL_DISPLAYPROPERTY_TYPE_EXPANSIONMODE = 1;
  ADL_DL_DISPLAYPROPERTY_TYPE_USEUNDERSCANSCALING = 2;
  /// Enables ITC processing for HDMI panels that are capable of the feature
  ADL_DL_DISPLAYPROPERTY_TYPE_ITCFLAGENABLE = 9;
  ADL_DL_DISPLAYPROPERTY_TYPE_DOWNSCALE = 11;

  /// Values for ADLDisplayContent.iContentType
  /// Certain HDMI panels that support ITC have support for a feature such that, the display on the panel
  /// can be adjusted to optimize the view of the content being displayed, depending on the type of content.
  ADL_DL_DISPLAYCONTENT_TYPE_GRAPHICS = 1;
  ADL_DL_DISPLAYCONTENT_TYPE_PHOTO = 2;
  ADL_DL_DISPLAYCONTENT_TYPE_CINEMA = 4;
  ADL_DL_DISPLAYCONTENT_TYPE_GAME = 8;

  //values for ADLDisplayProperty.iExpansionMode
  ADL_DL_DISPLAYPROPERTY_EXPANSIONMODE_CENTER = 0;
  ADL_DL_DISPLAYPROPERTY_EXPANSIONMODE_FULLSCREEN = 1;
  ADL_DL_DISPLAYPROPERTY_EXPANSIONMODE_ASPECTRATIO = 2;

  ///\defgroup define_dither_states Dithering options
  // @{
  /// Dithering disabled.
  ADL_DL_DISPLAY_DITHER_DISABLED = 0;
  /// Use default driver settings for dithering. Note that the default setting could be dithering disabled.
  ADL_DL_DISPLAY_DITHER_DRIVER_DEFAULT = 1;
  /// Temporal dithering to 6 bpc. Note that if the input is 12 bits, the two least significant bits will be truncated.
  ADL_DL_DISPLAY_DITHER_FM6 = 2;
  /// Temporal dithering to 8 bpc.
  ADL_DL_DISPLAY_DITHER_FM8 = 3;
  /// Temporal dithering to 10 bpc.
  ADL_DL_DISPLAY_DITHER_FM10 = 4;
  /// Spatial dithering to 6 bpc. Note that if the input is 12 bits, the two least significant bits will be truncated.
  ADL_DL_DISPLAY_DITHER_DITH6 = 5;
  /// Spatial dithering to 8 bpc.
  ADL_DL_DISPLAY_DITHER_DITH8 = 6;
  /// Spatial dithering to 10 bpc.
  ADL_DL_DISPLAY_DITHER_DITH10 = 7;
  /// Spatial dithering to 6 bpc. Random number generators are reset every frame, so the same input value of a certain pixel will always be dithered to the same output value. Note that if the input is 12 bits, the two least significant bits will be truncated.
  ADL_DL_DISPLAY_DITHER_DITH6_NO_FRAME_RAND = 8;
  /// Spatial dithering to 8 bpc. Random number generators are reset every frame, so the same input value of a certain pixel will always be dithered to the same output value.
  ADL_DL_DISPLAY_DITHER_DITH8_NO_FRAME_RAND = 9;
  /// Spatial dithering to 10 bpc. Random number generators are reset every frame, so the same input value of a certain pixel will always be dithered to the same output value.
  ADL_DL_DISPLAY_DITHER_DITH10_NO_FRAME_RAND = 10;
  /// Truncation to 6 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN6 = 11;
  /// Truncation to 8 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN8 = 12;
  /// Truncation to 10 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN10 = 13;
  /// Truncation to 10 bpc followed by spatial dithering to 8 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN10_DITH8 = 14;
  /// Truncation to 10 bpc followed by spatial dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN10_DITH6 = 15;
  /// Truncation to 10 bpc followed by temporal dithering to 8 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN10_FM8 = 16;
  /// Truncation to 10 bpc followed by temporal dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN10_FM6 = 17;
  /// Truncation to 10 bpc followed by spatial dithering to 8 bpc and temporal dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN10_DITH8_FM6 = 18;
  /// Spatial dithering to 10 bpc followed by temporal dithering to 8 bpc.
  ADL_DL_DISPLAY_DITHER_DITH10_FM8 = 19;
  /// Spatial dithering to 10 bpc followed by temporal dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_DITH10_FM6 = 20;
  /// Truncation to 8 bpc followed by spatial dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN8_DITH6 = 21;
  /// Truncation to 8 bpc followed by temporal dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_TRUN8_FM6 = 22;
  /// Spatial dithering to 8 bpc followed by temporal dithering to 6 bpc.
  ADL_DL_DISPLAY_DITHER_DITH8_FM6 = 23;
  ADL_DL_DISPLAY_DITHER_LAST = ADL_DL_DISPLAY_DITHER_DITH8_FM6;
  // @}

  /// Display Get Cached EDID flag
  ADL_MAX_EDIDDATA_SIZE = 256; // number of UCHAR;
  ADL_MAX_OVERRIDEEDID_SIZE = 512; // number of UCHAR;
  ADL_MAX_EDID_EXTENSION_BLOCKS = 3;

  ADL_DL_CONTROLLER_OVERLAY_ALPHA = 0;
  ADL_DL_CONTROLLER_OVERLAY_ALPHAPERPIX = 1;

  ADL_DL_DISPLAY_DATA_PACKET__INFO_PACKET_RESET = $00000000;
  ADL_DL_DISPLAY_DATA_PACKET__INFO_PACKET_SET = $00000001;
  ADL_DL_DISPLAY_DATA_PACKET__INFO_PACKET_SCAN = $00000002;

  ///\defgroup define_display_packet Display Data Packet Types
  // @{
  ADL_DL_DISPLAY_DATA_PACKET__TYPE__AVI = $00000001;
  ADL_DL_DISPLAY_DATA_PACKET__TYPE__GAMMUT = $00000002;
  ADL_DL_DISPLAY_DATA_PACKET__TYPE__VENDORINFO = $00000004;
  ADL_DL_DISPLAY_DATA_PACKET__TYPE__HDR = $00000008;
  ADL_DL_DISPLAY_DATA_PACKET__TYPE__SPD = $00000010;
  // @}

  // matrix types
  ADL_GAMUT_MATRIX_SD = 1; // SD matrix i.e. BT601;
  ADL_GAMUT_MATRIX_HD = 2; // HD matrix i.e. BT709;

  ///\defgroup define_clockinfo_flags Clock flags
  /// Used by ADLAdapterODClockInfo.iFlag
  // @{
  ADL_DL_CLOCKINFO_FLAG_FULLSCREEN3DONLY = $00000001;
  ADL_DL_CLOCKINFO_FLAG_ALWAYSFULLSCREEN3D = $00000002;
  ADL_DL_CLOCKINFO_FLAG_VPURECOVERYREDUCED = $00000004;
  ADL_DL_CLOCKINFO_FLAG_THERMALPROTECTION = $00000008;
  // @}

  // Supported GPUs
  // ADL_Display_PowerXpressActiveGPU_Get()
  ADL_DL_POWERXPRESS_GPU_INTEGRATED = 1;
  ADL_DL_POWERXPRESS_GPU_DISCRETE = 2;

  // Possible values for lpOperationResult
  // ADL_Display_PowerXpressActiveGPU_Get()
  ADL_DL_POWERXPRESS_SWITCH_RESULT_STARTED = 1; // Switch procedure has been started - Windows platform only;
  ADL_DL_POWERXPRESS_SWITCH_RESULT_DECLINED = 2; // Switch procedure cannot be started - All platforms;
  ADL_DL_POWERXPRESS_SWITCH_RESULT_ALREADY = 3; // System already has required status - All platforms;
  ADL_DL_POWERXPRESS_SWITCH_RESULT_DEFERRED = 5;
  // Switch was deferred and requires an X restart - Linux platform only;

  // PowerXpress support version
  // ADL_Display_PowerXpressVersion_Get()
  ADL_DL_POWERXPRESS_VERSION_MAJOR = 2; // Current PowerXpress support version 2.0;
  ADL_DL_POWERXPRESS_VERSION_MINOR = 0;

  ADL_DL_POWERXPRESS_VERSION = (((ADL_DL_POWERXPRESS_VERSION_MAJOR) shl 16) or ADL_DL_POWERXPRESS_VERSION_MINOR);

  //values for ADLThermalControllerInfo.iThermalControllerDomain
  ADL_DL_THERMAL_DOMAIN_OTHER = 0;
  ADL_DL_THERMAL_DOMAIN_GPU = 1;

  //values for ADLThermalControllerInfo.iFlags
  ADL_DL_THERMAL_FLAG_INTERRUPT = 1;
  ADL_DL_THERMAL_FLAG_FANCONTROL = 2;

  ///\defgroup define_fanctrl Fan speed cotrol
  /// Values for ADLFanSpeedInfo.iFlags
  // @{
  ADL_DL_FANCTRL_SUPPORTS_PERCENT_READ = 1;
  ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE = 2;
  ADL_DL_FANCTRL_SUPPORTS_RPM_READ = 4;
  ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE = 8;
  // @}

  //values for ADLFanSpeedValue.iSpeedType
  ADL_DL_FANCTRL_SPEED_TYPE_PERCENT = 1;
  ADL_DL_FANCTRL_SPEED_TYPE_RPM = 2;

  //values for ADLFanSpeedValue.iFlags
  ADL_DL_FANCTRL_FLAG_USER_DEFINED_SPEED = 1;

  // MVPU interfaces
  ADL_DL_MAX_MVPU_ADAPTERS = 4;
  MVPU_ADAPTER_0 = $00000001;
  MVPU_ADAPTER_1 = $00000002;
  MVPU_ADAPTER_2 = $00000004;
  MVPU_ADAPTER_3 = $00000008;
  ADL_DL_MAX_REGISTRY_PATH = 256;

  //values for ADLMVPUStatus.iStatus
  ADL_DL_MVPU_STATUS_OFF = 0;
  ADL_DL_MVPU_STATUS_ON = 1;

  // values for ASIC family
  ///\defgroup define_Asic_type Detailed asic types
  /// Defines for Adapter ASIC family type
  // @{
  ADL_ASIC_UNDEFINED = 0;
  ADL_ASIC_DISCRETE = (1 shl 0);
  ADL_ASIC_INTEGRATED = (1 shl 1);
  ADL_ASIC_FIREGL = (1 shl 2);
  ADL_ASIC_FIREMV = (1 shl 3);
  ADL_ASIC_XGP = (1 shl 4);
  ADL_ASIC_FUSION = (1 shl 5);
  ADL_ASIC_FIRESTREAM = (1 shl 6);
  ADL_ASIC_EMBEDDED = (1 shl 7);
  // @}

  ///\defgroup define_detailed_timing_flags Detailed Timimg Flags
  /// Defines for ADLDetailedTiming.sTimingFlags field
  // @{
  ADL_DL_TIMINGFLAG_DOUBLE_SCAN = $0001;
  //sTimingFlags is set when the mode is INTERLACED, if not PROGRESSIVE
  ADL_DL_TIMINGFLAG_INTERLACED = $0002;
  //sTimingFlags is set when the Horizontal Sync is POSITIVE, if not NEGATIVE
  ADL_DL_TIMINGFLAG_H_SYNC_POLARITY = $0004;
  //sTimingFlags is set when the Vertical Sync is POSITIVE, if not NEGATIVE
  ADL_DL_TIMINGFLAG_V_SYNC_POLARITY = $0008;
  // @}

  ///\defgroup define_modetiming_standard Timing Standards
  /// Defines for ADLDisplayModeInfo.iTimingStandard field
  // @{
  ADL_DL_MODETIMING_STANDARD_CVT = $00000001; // CVT Standard;
  ADL_DL_MODETIMING_STANDARD_GTF = $00000002; // GFT Standard;
  ADL_DL_MODETIMING_STANDARD_DMT = $00000004; // DMT Standard;
  ADL_DL_MODETIMING_STANDARD_CUSTOM = $00000008; // User-defined standard;
  ADL_DL_MODETIMING_STANDARD_DRIVER_DEFAULT = $00000010; // Remove Mode from overriden list;
  ADL_DL_MODETIMING_STANDARD_CVT_RB = $00000020; // CVT-RB Standard;
  // @}

  // \defgroup define_xserverinfo driver x-server info
  /// These flags are used by ADL_XServerInfo_Get()
  // @

  /// Xinerama is active in the x-server, Xinerama extension may report it to be active but it
  /// may not be active in x-server
  ADL_XSERVERINFO_XINERAMAACTIVE = (1 shl 0);

  /// RandR 1.2 is supported by driver, RandR extension may report version 1.2
  /// but driver may not support it
  ADL_XSERVERINFO_RANDR12SUPPORTED = (1 shl 1);
  // @

  ///\defgroup define_eyefinity_constants Eyefinity Definitions
  // @{

  ADL_CONTROLLERVECTOR_0 = 1; // ADL_CONTROLLERINDEX_0 = 0, (1 shl ADL_CONTROLLERINDEX_0);
  ADL_CONTROLLERVECTOR_1 = 2; // ADL_CONTROLLERINDEX_1 = 1, (1 shl ADL_CONTROLLERINDEX_1);

  ADL_DISPLAY_SLSGRID_ORIENTATION_000 = $00000001;
  ADL_DISPLAY_SLSGRID_ORIENTATION_090 = $00000002;
  ADL_DISPLAY_SLSGRID_ORIENTATION_180 = $00000004;
  ADL_DISPLAY_SLSGRID_ORIENTATION_270 = $00000008;
  ADL_DISPLAY_SLSGRID_CAP_OPTION_RELATIVETO_LANDSCAPE = $00000001;
  ADL_DISPLAY_SLSGRID_CAP_OPTION_RELATIVETO_CURRENTANGLE = $00000002;
  ADL_DISPLAY_SLSGRID_PORTAIT_MODE = $00000004;
  ADL_DISPLAY_SLSGRID_KEEPTARGETROTATION = $00000080;

  ADL_DISPLAY_SLSGRID_SAMEMODESLS_SUPPORT = $00000010;
  ADL_DISPLAY_SLSGRID_MIXMODESLS_SUPPORT = $00000020;
  ADL_DISPLAY_SLSGRID_DISPLAYROTATION_SUPPORT = $00000040;
  ADL_DISPLAY_SLSGRID_DESKTOPROTATION_SUPPORT = $00000080;

  ADL_DISPLAY_SLSMAP_SLSLAYOUTMODE_FIT = $0100;
  ADL_DISPLAY_SLSMAP_SLSLAYOUTMODE_FILL = $0200;
  ADL_DISPLAY_SLSMAP_SLSLAYOUTMODE_EXPAND = $0400;

  ADL_DISPLAY_SLSMAP_IS_SLS = $1000;
  ADL_DISPLAY_SLSMAP_IS_SLSBUILDER = $2000;
  ADL_DISPLAY_SLSMAP_IS_CLONEVT = $4000;

  ADL_DISPLAY_SLSMAPCONFIG_GET_OPTION_RELATIVETO_LANDSCAPE = $00000001;
  ADL_DISPLAY_SLSMAPCONFIG_GET_OPTION_RELATIVETO_CURRENTANGLE = $00000002;

  ADL_DISPLAY_SLSMAPCONFIG_CREATE_OPTION_RELATIVETO_LANDSCAPE = $00000001;
  ADL_DISPLAY_SLSMAPCONFIG_CREATE_OPTION_RELATIVETO_CURRENTANGLE = $00000002;

  ADL_DISPLAY_SLSMAPCONFIG_REARRANGE_OPTION_RELATIVETO_LANDSCAPE = $00000001;
  ADL_DISPLAY_SLSMAPCONFIG_REARRANGE_OPTION_RELATIVETO_CURRENTANGLE = $00000002;

  ADL_SLS_SAMEMODESLS_SUPPORT = $0001;
  ADL_SLS_MIXMODESLS_SUPPORT = $0002;
  ADL_SLS_DISPLAYROTATIONSLS_SUPPORT = $0004;
  ADL_SLS_DESKTOPROTATIONSLS_SUPPORT = $0008;

  ADL_SLS_TARGETS_INVALID = $0001;
  ADL_SLS_MODES_INVALID = $0002;
  ADL_SLS_ROTATIONS_INVALID = $0004;
  ADL_SLS_POSITIONS_INVALID = $0008;
  ADL_SLS_LAYOUTMODE_INVALID = $0010;

  ADL_DISPLAY_SLSDISPLAYOFFSET_VALID = $0002;

  ADL_DISPLAY_SLSGRID_RELATIVETO_LANDSCAPE = $00000010;
  ADL_DISPLAY_SLSGRID_RELATIVETO_CURRENTANGLE = $00000020;

  /// The bit mask identifies displays is currently in bezel mode.
  ADL_DISPLAY_SLSMAP_BEZELMODE = $00000010;
  /// The bit mask identifies displays from this map is arranged.
  ADL_DISPLAY_SLSMAP_DISPLAYARRANGED = $00000002;
  /// The bit mask identifies this map is currently in used for the current adapter.
  ADL_DISPLAY_SLSMAP_CURRENTCONFIG = $00000004;

  ///For onlay active SLS  map info
  ADL_DISPLAY_SLSMAPINDEXLIST_OPTION_ACTIVE = $00000001;

  ///For Bezel
  ADL_DISPLAY_BEZELOFFSET_STEPBYSTEPSET = $00000004;
  ADL_DISPLAY_BEZELOFFSET_COMMIT = $00000008;

type
  SLS_ImageCropType = (
    Fit = 1,
    Fill = 2,
    Expand = 3);

type
  DceSettingsType = (
    DceSetting_HdmiLq,
    DceSetting_DpSettings,
    DceSetting_Protection);

type
  DpLinkRate = (
    DPLinkRate_Unknown,
    DPLinkRate_RBR,
    DPLinkRate_HBR,
    DPLinkRate_HBR2,
    DPLinkRate_HBR3);

// @}

///\defgroup define_powerxpress_constants PowerXpress Definitions
/// @{

/// The bit mask identifies PX caps for ADLPXConfigCaps.iPXConfigCapMask and ADLPXConfigCaps.iPXConfigCapValue
const

  ADL_PX_CONFIGCAPS_SPLASHSCREEN_SUPPORT = $0001;
  ADL_PX_CONFIGCAPS_CF_SUPPORT = $0002;
  ADL_PX_CONFIGCAPS_MUXLESS = $0004;
  ADL_PX_CONFIGCAPS_PROFILE_COMPLIANT = $0008;
  ADL_PX_CONFIGCAPS_NON_AMD_DRIVEN_DISPLAYS = $0010;
  ADL_PX_CONFIGCAPS_FIXED_SUPPORT = $0020;
  ADL_PX_CONFIGCAPS_DYNAMIC_SUPPORT = $0040;
  ADL_PX_CONFIGCAPS_HIDE_AUTO_SWITCH = $0080;

  /// The bit mask identifies PX schemes for ADLPXSchemeRange
  ADL_PX_SCHEMEMASK_FIXED = $0001;
  ADL_PX_SCHEMEMASK_DYNAMIC = $0002;

/// PX Schemes
type
  ADLPXScheme = (
    ADL_PX_SCHEME_INVALID = 0,
    ADL_PX_SCHEME_FIXED = ADL_PX_SCHEMEMASK_FIXED,
    ADL_PX_SCHEME_DYNAMIC = ADL_PX_SCHEMEMASK_DYNAMIC);

/// Just keep the old definitions for compatibility, need to be removed later
type
  PXScheme = (
    PX_SCHEME_INVALID = 0,
    PX_SCHEME_FIXED = 1,
    PX_SCHEME_DYNAMIC = 2);

/// @}

///\defgroup define_appprofiles For Application Profiles
/// @{
const
  ADL_APP_PROFILE_FILENAME_LENGTH = 64;
  ADL_APP_PROFILE_TIMESTAMP_LENGTH = 32;
  ADL_APP_PROFILE_VERSION_LENGTH = 32;
  ADL_APP_PROFILE_PROPERTY_LENGTH = 64;

type
  ApplicationListType = (
    ADL_PX40_MRU,
    ADL_PX40_MISSED,
    ADL_PX40_DISCRETE,
    ADL_PX40_INTEGRATED,
    ADL_MMD_PROFILED,
    ADL_PX40_TOTAL);

type
  ADLProfilePropertyType = (
    ADL_PROFILEPROPERTY_TYPE_BINARY = 0,
    ADL_PROFILEPROPERTY_TYPE_BOOLEAN,
    ADL_PROFILEPROPERTY_TYPE_DWORD,
    ADL_PROFILEPROPERTY_TYPE_QWORD,
    ADL_PROFILEPROPERTY_TYPE_ENUMERATED,
    ADL_PROFILEPROPERTY_TYPE_STRING);

/// @}

///\defgroup define_dp12 For Display Port 1.2
/// @{

/// Maximum Relative Address Link
const
  ADL_MAX_RAD_LINK_COUNT = 15;

  /// @}

  ///\defgroup defines_gamutspace Driver Supported Gamut Space
  /// @{

  /// The flags desribes that gamut is related to source or to destination and to overlay or to graphics
  ADL_GAMUT_REFERENCE_SOURCE = (1 shl 0);
  ADL_GAMUT_GAMUT_VIDEO_CONTENT = (1 shl 1);

  /// The flags are used to describe the source of gamut and how read information from struct ADLGamutData
  ADL_CUSTOM_WHITE_POINT = (1 shl 0);
  ADL_CUSTOM_GAMUT = (1 shl 1);
  ADL_GAMUT_REMAP_ONLY = (1 shl 2);

  /// The define means the predefined gamut values  .
  ///Driver uses to find entry in the table and apply appropriate gamut space.
  ADL_GAMUT_SPACE_CCIR_709 = (1 shl 0);
  ADL_GAMUT_SPACE_CCIR_601 = (1 shl 1);
  ADL_GAMUT_SPACE_ADOBE_RGB = (1 shl 2);
  ADL_GAMUT_SPACE_CIE_RGB = (1 shl 3);
  ADL_GAMUT_SPACE_CUSTOM = (1 shl 4);
  ADL_GAMUT_SPACE_CCIR_2020 = (1 shl 5);
  ADL_GAMUT_SPACE_APPCTRL = (1 shl 6);

  /// Predefine white point values are structed similar to gamut .
  ADL_WHITE_POINT_5000K = (1 shl 0);
  ADL_WHITE_POINT_6500K = (1 shl 1);
  ADL_WHITE_POINT_7500K = (1 shl 2);
  ADL_WHITE_POINT_9300K = (1 shl 3);
  ADL_WHITE_POINT_CUSTOM = (1 shl 4);

  ///gamut and white point coordinates are from 0.0 -1.0 and divider is used to find the real value .
  /// X float = X int /divider
  ADL_GAMUT_WHITEPOINT_DIVIDER = 10000;

  ///gamma a0 coefficient uses the following divider:
  ADL_REGAMMA_COEFFICIENT_A0_DIVIDER = 10000000;
  ///gamma a1 ,a2,a3 coefficients use the following divider:
  ADL_REGAMMA_COEFFICIENT_A1A2A3_DIVIDER = 1000;

  ///describes whether the coefficients are from EDID or custom user values.
  ADL_EDID_REGAMMA_COEFFICIENTS = (1 shl 0);
  ///Used for struct ADLRegamma. Feature if set use gamma ramp, if missing use regamma coefficents
  ADL_USE_GAMMA_RAMP = (1 shl 4);
  ///Used for struct ADLRegamma. If the gamma ramp flag is used then the driver could apply de gamma corretion to the supplied curve and this depends on this flag
  ADL_APPLY_DEGAMMA = (1 shl 5);
  ///specifies that standard SRGB gamma should be applied
  ADL_EDID_REGAMMA_PREDEFINED_SRGB = (1 shl 1);
  ///specifies that PQ gamma curve should be applied
  ADL_EDID_REGAMMA_PREDEFINED_PQ = (1 shl 2);
  ///specifies that PQ gamma curve should be applied, lower max nits
  ADL_EDID_REGAMMA_PREDEFINED_PQ_2084_INTERIM = (1 shl 3);
  ///specifies that 3.6 gamma should be applied
  ADL_EDID_REGAMMA_PREDEFINED_36 = (1 shl 6);
  ///specifies that BT709 gama should be applied
  ADL_EDID_REGAMMA_PREDEFINED_BT709 = (1 shl 7);
  ///specifies that regamma should be disabled, and application controls regamma content (of the whole screen)
  ADL_EDID_REGAMMA_PREDEFINED_APPCTRL = (1 shl 8);

  /// @}

  /// \defgroup define_ddcinfo_pixelformats DDCInfo Pixel Formats
  /// @{
  /// defines for iPanelPixelFormat  in struct ADLDDCInfo2
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB656 = $00000001;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB666 = $00000002;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB888 = $00000004;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB101010 = $00000008;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB161616 = $00000010;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB_RESERVED1 = $00000020;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB_RESERVED2 = $00000040;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_RGB_RESERVED3 = $00000080;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_XRGB_BIAS101010 = $00000100;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR444_8BPCC = $00000200;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR444_10BPCC = $00000400;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR444_12BPCC = $00000800;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR422_8BPCC = $00001000;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR422_10BPCC = $00002000;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR422_12BPCC = $00004000;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR420_8BPCC = $00008000;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR420_10BPCC = $00010000;
  ADL_DISPLAY_DDCINFO_PIXEL_FORMAT_YCBCR420_12BPCC = $00020000;
  /// @}

  /// \defgroup define_source_content_TF ADLSourceContentAttributes transfer functions (gamma)
  /// @{
  /// defines for iTransferFunction in ADLSourceContentAttributes
  ADL_TF_sRGB = $0001; ///< sRGB;
  ADL_TF_BT709 = $0002; ///< BT.709;
  ADL_TF_PQ2084 = $0004; ///< PQ2084;
  ADL_TF_PQ2084_INTERIM = $0008; ///< PQ2084-Interim;
  ADL_TF_LINEAR_0_1 = $0010; ///< Linear 0 - 1;
  ADL_TF_LINEAR_0_125 = $0020; ///< Linear 0 - 125;
  ADL_TF_DOLBYVISION = $0040; ///< DolbyVision;
  ADL_TF_GAMMA_22 = $0080; ///< Plain 2.2 gamma curve;
  /// @}

  /// \defgroup define_source_content_CS ADLSourceContentAttributes color spaces
  /// @{
  /// defines for iColorSpace in ADLSourceContentAttributes
  ADL_CS_sRGB = $0001; ///< sRGB;
  ADL_CS_BT601 = $0002; ///< BT.601;
  ADL_CS_BT709 = $0004; ///< BT.709;
  ADL_CS_BT2020 = $0008; ///< BT.2020;
  ADL_CS_ADOBE = $0010; ///< Adobe RGB;
  ADL_CS_P3 = $0020; ///< DCI-P3;
  ADL_CS_scRGB_MS_REF = $0040; ///< scRGB (MS Reference);
  ADL_CS_DISPLAY_NATIVE = $0080; ///< Display Native;
  ADL_CS_APP_CONTROL = $0100; ///< Application Controlled;
  ADL_CS_DOLBYVISION = $0200; ///< DolbyVision;
  /// @}

  /// \defgroup define_HDR_support ADLDDCInfo2 HDR support options
  /// @{
  /// defines for iSupportedHDR in ADLDDCInfo2
  ADL_HDR_CEA861_3 = $0001; ///< HDR10/CEA861.3 HDR supported;
  ADL_HDR_DOLBYVISION = $0002; ///< DolbyVision HDR supported;
  ADL_HDR_FREESYNC_HDR = $0004; ///< FreeSync HDR supported;
  /// @}

  /// \defgroup define_FreesyncFlags ADLDDCInfo2 Freesync HDR flags
  /// @{
  /// defines for iFreesyncFlags in ADLDDCInfo2
  ADL_HDR_FREESYNC_BACKLIGHT_SUPPORT = $0001; ///< Backlight supported;
  /// @}

  /// \defgroup define_dbd_state Deep Bit Depth
  /// @{

  /// defines for ADL_Workstation_DeepBitDepth_Get and  ADL_Workstation_DeepBitDepth_Set functions
  // This value indicates that the deep bit depth state is forced off
  ADL_DEEPBITDEPTH_FORCEOFF = 0;
  /// This value indicates that the deep bit depth state  is set to auto, the driver will automatically enable the
  /// appropriate deep bit depth state depending on what connected display supports.
  ADL_DEEPBITDEPTH_10BPP_AUTO = 1;
  /// This value indicates that the deep bit depth state  is forced on to 10 bits per pixel, this is regardless if the display
  /// supports 10 bpp.
  ADL_DEEPBITDEPTH_10BPP_FORCEON = 2;

  /// defines for ADLAdapterConfigMemory of ADL_Adapter_ConfigMemory_Get
  /// If this bit is set, it indicates that the Deep Bit Depth pixel is set on the display
  ADL_ADAPTER_CONFIGMEMORY_DBD = (1 shl 0);
  /// If this bit is set, it indicates that the display is rotated (90, 180 or 270)
  ADL_ADAPTER_CONFIGMEMORY_ROTATE = (1 shl 1);
  /// If this bit is set, it indicates that passive stereo is set on the display
  ADL_ADAPTER_CONFIGMEMORY_STEREO_PASSIVE = (1 shl 2);
  /// If this bit is set, it indicates that the active stereo is set on the display
  ADL_ADAPTER_CONFIGMEMORY_STEREO_ACTIVE = (1 shl 3);
  /// If this bit is set, it indicates that the tear free vsync is set on the display
  ADL_ADAPTER_CONFIGMEMORY_ENHANCEDVSYNC = (1 shl 4);
  ADL_ADAPTER_CONFIGMEMORY_TEARFREEVSYNC = (1 shl 4);
  /// @}

  /// \defgroup define_adl_validmemoryrequiredfields Memory Type
  /// @{

  ///  This group defines memory types in ADLMemoryRequired struct \n
  /// Indicates that this is the visible memory
  ADL_MEMORYREQTYPE_VISIBLE = (1 shl 0);
  /// Indicates that this is the invisible memory.
  ADL_MEMORYREQTYPE_INVISIBLE = (1 shl 1);
  /// Indicates that this is amount of visible memory per GPU that should be reserved for all other allocations.
  ADL_MEMORYREQTYPE_GPURESERVEDVISIBLE = (1 shl 2);
  /// @}

  /// \defgroup define_adapter_tear_free_status
  /// Used in ADL_Adapter_TEAR_FREE_Set and ADL_Adapter_TFD_Get functions to indicate the tear free
  /// desktop status.
  /// @{
  /// Tear free desktop is enabled.
  ADL_ADAPTER_TEAR_FREE_ON = 1;
  /// Tear free desktop can't be enabled due to a lack of graphic adapter memory.
  ADL_ADAPTER_TEAR_FREE_NOTENOUGHMEM = -1;
  /// Tear free desktop can't be enabled due to quad buffer stereo being enabled.
  ADL_ADAPTER_TEAR_FREE_OFF_ERR_QUADBUFFERSTEREO = -2;
  /// Tear free desktop can't be enabled due to MGPU-SLS being enabled.
  ADL_ADAPTER_TEAR_FREE_OFF_ERR_MGPUSLD = -3;
  /// Tear free desktop is disabled.
  ADL_ADAPTER_TEAR_FREE_OFF = 0;
  /// @}

  /// \defgroup define_adapter_crossdisplay_platforminfo
  /// Used in ADL_Adapter_CrossDisplayPlatformInfo_Get function to indicate the Crossdisplay platform info.
  /// @{
  /// CROSSDISPLAY platform.
  ADL_CROSSDISPLAY_PLATFORM = (1 shl 0);
  /// CROSSDISPLAY platform for Lasso station.
  ADL_CROSSDISPLAY_PLATFORM_LASSO = (1 shl 1);
  /// CROSSDISPLAY platform for docking station.
  ADL_CROSSDISPLAY_PLATFORM_DOCKSTATION = (1 shl 2);
  /// @}

  /// \defgroup define_adapter_crossdisplay_option
  /// Used in ADL_Adapter_CrossdisplayInfoX2_Set function to indicate cross display options.
  /// @{
  /// Checking if 3D application is runnning. If yes, not to do switch, return ADL_OK_WAIT; otherwise do switch.
  ADL_CROSSDISPLAY_OPTION_NONE = 0;
  /// Force switching without checking for running 3D applications
  ADL_CROSSDISPLAY_OPTION_FORCESWITCH = (1 shl 0);
  /// @}

  /// \defgroup define_adapter_states Adapter Capabilities
  /// These defines the capabilities supported by an adapter. It is used by \ref ADL_Adapter_ConfigureState_Get
  /// @{
  /// Indicates that the adapter is headless (i.e. no displays can be connected to it)
  ADL_ADAPTERCONFIGSTATE_HEADLESS = (1 shl 2);
  /// Indicates that the adapter is configured to define the main rendering capabilities. For example, adapters
  /// in Crossfire(TM) configuration, this bit would only be set on the adapter driving the display(s).
  ADL_ADAPTERCONFIGSTATE_REQUISITE_RENDER = (1 shl 0);
  /// Indicates that the adapter is configured to be used to unload some of the rendering work for a particular
  /// requisite rendering adapter. For eample, for adapters in a Crossfire configuration, this bit would be set
  /// on all adapters that are currently not driving the display(s)
  ADL_ADAPTERCONFIGSTATE_ANCILLARY_RENDER = (1 shl 1);
  /// Indicates that scatter gather feature enabled on the adapter
  ADL_ADAPTERCONFIGSTATE_SCATTERGATHER = (1 shl 4);
  /// @}

  /// \defgroup define_controllermode_ulModifiers
  /// These defines the detailed actions supported by set viewport. It is used by \ref ADL_Display_ViewPort_Set
  /// @{
  /// Indicate that the viewport set will change the view position
  ADL_CONTROLLERMODE_CM_MODIFIER_VIEW_POSITION = $00000001;
  /// Indicate that the viewport set will change the view PanLock
  ADL_CONTROLLERMODE_CM_MODIFIER_VIEW_PANLOCK = $00000002;
  /// Indicate that the viewport set will change the view size
  ADL_CONTROLLERMODE_CM_MODIFIER_VIEW_SIZE = $00000008;
  /// @}

  /// \defgroup defines for Mirabilis
  /// These defines are used for the Mirabilis feature
  /// @{
  ///
  /// Indicates the maximum number of audio sample rates
  ADL_MAX_AUDIO_SAMPLE_RATE_COUNT = 16;
/// @}

///////////////////////////////////////////////////////////////////////////
// ADLMultiChannelSplitStateFlag Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLMultiChannelSplitStateFlag = (
    ADLMultiChannelSplit_Unitialized = 0,
    ADLMultiChannelSplit_Disabled = 1,
    ADLMultiChannelSplit_Enabled = 2,
    ADLMultiChannelSplit_SaveProfile = 3);

///////////////////////////////////////////////////////////////////////////
// ADLSampleRate Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLSampleRate = (
    ADLSampleRate_32KHz = 0,
    ADLSampleRate_44P1KHz,
    ADLSampleRate_48KHz,
    ADLSampleRate_88P2KHz,
    ADLSampleRate_96KHz,
    ADLSampleRate_176P4KHz,
    ADLSampleRate_192KHz,
    ADLSampleRate_384KHz, //DP1.2;
    ADLSampleRate_768KHz, //DP1.2;
    ADLSampleRate_Undefined);

/// \defgroup define_overdrive6_capabilities
/// These defines the capabilities supported by Overdrive 6. It is used by \ref ADL_Overdrive6_Capabilities_Get
// @{
/// Indicate that core (engine) clock can be changed.
const
  ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION = $00000001;
  /// Indicate that memory clock can be changed.
  ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION = $00000002;
  /// Indicate that graphics activity reporting is supported.
  ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR = $00000004;
  /// Indicate that power limit can be customized.
  ADL_OD6_CAPABILITY_POWER_CONTROL = $00000008;
  /// Indicate that SVI2 Voltage Control is supported.
  ADL_OD6_CAPABILITY_VOLTAGE_CONTROL = $00000010;
  /// Indicate that OD6+ percentage adjustment is supported.
  ADL_OD6_CAPABILITY_PERCENT_ADJUSTMENT = $00000020;
  /// Indicate that Thermal Limit Unlock is supported.
  ADL_OD6_CAPABILITY_THERMAL_LIMIT_UNLOCK = $00000040;
  ///Indicate that Fan speed needs to be displayed in RPM
  ADL_OD6_CAPABILITY_FANSPEED_IN_RPM = $00000080;
  // @}

  /// \defgroup define_overdrive6_supported_states
  /// These defines the power states supported by Overdrive 6. It is used by \ref ADL_Overdrive6_Capabilities_Get
  // @{
  /// Indicate that overdrive is supported in the performance state.  This is currently the only state supported.
  ADL_OD6_SUPPORTEDSTATE_PERFORMANCE = $00000001;
  /// Do not use.  Reserved for future use.
  ADL_OD6_SUPPORTEDSTATE_POWER_SAVING = $00000002;
  // @}

  /// \defgroup define_overdrive6_getstateinfo
  /// These defines the power states to get information about. It is used by \ref ADL_Overdrive6_StateInfo_Get
  // @{
  /// Get default clocks for the performance state.
  ADL_OD6_GETSTATEINFO_DEFAULT_PERFORMANCE = $00000001;
  /// Do not use.  Reserved for future use.
  ADL_OD6_GETSTATEINFO_DEFAULT_POWER_SAVING = $00000002;
  /// Get clocks for current state.  Currently this is the same as \ref ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE
  /// since only performance state is supported.
  ADL_OD6_GETSTATEINFO_CURRENT = $00000003;
  /// Get the modified clocks (if any) for the performance state.  If clocks were not modified
  /// through Overdrive 6, then this will return the same clocks as \ref ADL_OD6_GETSTATEINFO_DEFAULT_PERFORMANCE.
  ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE = $00000004;
  /// Do not use.  Reserved for future use.
  ADL_OD6_GETSTATEINFO_CUSTOM_POWER_SAVING = $00000005;
  // @}

  /// \defgroup define_overdrive6_getstate and define_overdrive6_getmaxclockadjust
  /// These defines the power states to get information about. It is used by \ref ADL_Overdrive6_StateEx_Get and \ref ADL_Overdrive6_MaxClockAdjust_Get
  // @{
  /// Get default clocks for the performance state.  Only performance state is currently supported.
  ADL_OD6_STATE_PERFORMANCE = $00000001;
  // @}

  /// \defgroup define_overdrive6_setstate
  /// These define which power state to set customized clocks on. It is used by \ref ADL_Overdrive6_State_Set
  // @{
  /// Set customized clocks for the performance state.
  ADL_OD6_SETSTATE_PERFORMANCE = $00000001;
  /// Do not use.  Reserved for future use.
  ADL_OD6_SETSTATE_POWER_SAVING = $00000002;
  // @}

  /// \defgroup define_overdrive6_thermalcontroller_caps
  /// These defines the capabilities of the GPU thermal controller. It is used by \ref ADL_Overdrive6_ThermalController_Caps
  // @{
  /// GPU thermal controller is supported.
  ADL_OD6_TCCAPS_THERMAL_CONTROLLER = $00000001;
  /// GPU fan speed control is supported.
  ADL_OD6_TCCAPS_FANSPEED_CONTROL = $00000002;
  /// Fan speed percentage can be read.
  ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ = $00000100;
  /// Fan speed can be set by specifying a percentage value.
  ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE = $00000200;
  /// Fan speed RPM (revolutions-per-minute) can be read.
  ADL_OD6_TCCAPS_FANSPEED_RPM_READ = $00000400;
  /// Fan speed can be set by specifying an RPM value.
  ADL_OD6_TCCAPS_FANSPEED_RPM_WRITE = $00000800;
  // @}

  /// \defgroup define_overdrive6_fanspeed_type
  /// These defines the fan speed type being reported. It is used by \ref ADL_Overdrive6_FanSpeed_Get
  // @{
  /// Fan speed reported in percentage.
  ADL_OD6_FANSPEED_TYPE_PERCENT = $00000001;
  /// Fan speed reported in RPM.
  ADL_OD6_FANSPEED_TYPE_RPM = $00000002;
  /// Fan speed has been customized by the user, and fan is not running in automatic mode.
  ADL_OD6_FANSPEED_USER_DEFINED = $00000100;
  // @}

  /// \defgroup define_overdrive_EventCounter_type
  /// These defines the EventCounter type being reported. It is used by \ref ADL2_OverdriveN_CountOfEvents_Get ,can be used on older OD version supported ASICs also.
  // @{
  ADL_ODN_EVENTCOUNTER_THERMAL = 0;
  ADL_ODN_EVENTCOUNTER_VPURECOVERY = 1;
// @}

///////////////////////////////////////////////////////////////////////////
// ADLODNControlType Enumeration
///////////////////////////////////////////////////////////////////////////
type
  ADLODNControlType = (
    ODNControlType_Current = 0,
    ODNControlType_Default,
    ODNControlType_Auto,
    ODNControlType_Manual);

/// \defgroup define_ecc_mode_states
/// These defines the ECC(Error Correction Code) state. It is used by \ref ADL_Workstation_ECC_Get,ADL_Workstation_ECC_Set
// @{
/// Error Correction is OFF.
const
  ECC_MODE_OFF = 0;
  /// Error Correction is ECCV2.
  ECC_MODE_ON = 2;
  /// Error Correction is HBM.
  ECC_MODE_HBM = 3;
  // @}

  /// \defgroup define_board_layout_flags
  /// These defines are the board layout flags state which indicates what are the valid properties of \ref ADLBoardLayoutInfo . It is used by \ref ADL_Adapter_BoardLayout_Get
  // @{
  /// Indicates the number of slots is valid.
  ADL_BLAYOUT_VALID_NUMBER_OF_SLOTS = $1;
  /// Indicates the slot sizes are valid. Size of the slot consists of the length and width.
  ADL_BLAYOUT_VALID_SLOT_SIZES = $2;
  /// Indicates the connector offsets are valid.
  ADL_BLAYOUT_VALID_CONNECTOR_OFFSETS = $4;
  /// Indicates the connector lengths is valid.
  ADL_BLAYOUT_VALID_CONNECTOR_LENGTHS = $8;
  // @}

  /// \defgroup define_max_constants
  /// These defines are the maximum value constants.
  // @{
  /// Indicates the Maximum supported slots on board.
  ADL_ADAPTER_MAX_SLOTS = 4;
  /// Indicates the Maximum supported connectors on slot.
  ADL_ADAPTER_MAX_CONNECTORS = 10;
  /// Indicates the Maximum supported properties of connection
  ADL_MAX_CONNECTION_TYPES = 32;
  /// Indicates the Maximum relative address link count.
  ADL_MAX_RELATIVE_ADDRESS_LINK_COUNT = 15;
  /// Indicates the Maximum size of EDID data block size
  ADL_MAX_DISPLAY_EDID_DATA_SIZE = 1024;
  /// Indicates the Maximum count of Error Records.
  ADL_MAX_ERROR_RECORDS_COUNT = 256;
  /// Indicates the maximum number of power states supported
  ADL_MAX_POWER_POLICY = 6;
  // @}

  /// \defgroup define_connection_types
  /// These defines are the connection types constants which indicates  what are the valid connection type of given connector. It is used by \ref ADL_Adapter_SupportedConnections_Get
  // @{
  /// Indicates the VGA connection type is valid.
  ADL_CONNECTION_TYPE_VGA = 0;
  /// Indicates the DVI_I connection type is valid.
  ADL_CONNECTION_TYPE_DVI = 1;
  /// Indicates the DVI_SL connection type is valid.
  ADL_CONNECTION_TYPE_DVI_SL = 2;
  /// Indicates the HDMI connection type is valid.
  ADL_CONNECTION_TYPE_HDMI = 3;
  /// Indicates the DISPLAY PORT connection type is valid.
  ADL_CONNECTION_TYPE_DISPLAY_PORT = 4;
  /// Indicates the Active dongle DP->DVI(single link) connection type is valid.
  ADL_CONNECTION_TYPE_ACTIVE_DONGLE_DP_DVI_SL = 5;
  /// Indicates the Active dongle DP->DVI(double link) connection type is valid.
  ADL_CONNECTION_TYPE_ACTIVE_DONGLE_DP_DVI_DL = 6;
  /// Indicates the Active dongle DP->HDMI connection type is valid.
  ADL_CONNECTION_TYPE_ACTIVE_DONGLE_DP_HDMI = 7;
  /// Indicates the Active dongle DP->VGA connection type is valid.
  ADL_CONNECTION_TYPE_ACTIVE_DONGLE_DP_VGA = 8;
  /// Indicates the Passive dongle DP->HDMI connection type is valid.
  ADL_CONNECTION_TYPE_PASSIVE_DONGLE_DP_HDMI = 9;
  /// Indicates the Active dongle DP->VGA connection type is valid.
  ADL_CONNECTION_TYPE_PASSIVE_DONGLE_DP_DVI = 10;
  /// Indicates the MST type is valid.
  ADL_CONNECTION_TYPE_MST = 11;
  /// Indicates the active dongle, all types.
  ADL_CONNECTION_TYPE_ACTIVE_DONGLE = 12;
  /// Indicates the Virtual Connection Type.
  ADL_CONNECTION_TYPE_VIRTUAL = 13;
  /// Macros for generating bitmask from index.
  //  ADL_CONNECTION_BITMAST_FROM_INDEX[index] = (1 shl index); OOPS
  // @}

  /// \defgroup define_connection_properties
  /// These defines are the connection properties which indicates what are the valid properties of given connection type. It is used by \ref ADL_Adapter_SupportedConnections_Get
  // @{
  /// Indicates the property Bitrate is valid.
  ADL_CONNECTION_PROPERTY_BITRATE = $1;
  /// Indicates the property number of lanes is valid.
  ADL_CONNECTION_PROPERTY_NUMBER_OF_LANES = $2;
  /// Indicates the property 3D caps is valid.
  ADL_CONNECTION_PROPERTY_3DCAPS = $4;
  /// Indicates the property output bandwidth is valid.
  ADL_CONNECTION_PROPERTY_OUTPUT_BANDWIDTH = $8;
  /// Indicates the property colordepth is valid.
  ADL_CONNECTION_PROPERTY_COLORDEPTH = $10;
  // @}

  /// \defgroup define_lanecount_constants
  /// These defines are the Lane count constants which will be used in DP & etc.
  // @{
  /// Indicates if lane count is unknown
  ADL_LANECOUNT_UNKNOWN = 0;
  /// Indicates if lane count is 1
  ADL_LANECOUNT_ONE = 1;
  /// Indicates if lane count is 2
  ADL_LANECOUNT_TWO = 2;
  /// Indicates if lane count is 4
  ADL_LANECOUNT_FOUR = 4;
  /// Indicates if lane count is 8
  ADL_LANECOUNT_EIGHT = 8;
  /// Indicates default value of lane count
  ADL_LANECOUNT_DEF = ADL_LANECOUNT_FOUR;
  // @}

  /// \defgroup define_linkrate_constants
  /// These defines are the link rate constants which will be used in DP & etc.
  // @{
  /// Indicates if link rate is unknown
  ADL_LINK_BITRATE_UNKNOWN = 0;
  /// Indicates if link rate is 1.62Ghz
  ADL_LINK_BITRATE_1_62_GHZ = $06;
  /// Indicates if link rate is 2.7Ghz
  ADL_LINK_BITRATE_2_7_GHZ = $0A;
  /// Indicates if link rate is 3.24Ghz
  ADL_LINK_BTIRATE_3_24_GHZ = $0C;
  /// Indicates if link rate is 5.4Ghz
  ADL_LINK_BITRATE_5_4_GHZ = $14;
  /// Indicates default value of link rate
  ADL_LINK_BITRATE_DEF = ADL_LINK_BITRATE_2_7_GHZ;
  // @}

  /// \defgroup define_colordepth_constants
  /// These defines are the color depth constants which will be used in DP & etc.
  // @{
  ADL_CONNPROP_S3D_ALTERNATE_TO_FRAME_PACK = $00000001;
  // @}

  /// \defgroup define_colordepth_constants
  /// These defines are the color depth constants which will be used in DP & etc.
  // @{
  /// Indicates if color depth is unknown
  ADL_COLORDEPTH_UNKNOWN = 0;
  /// Indicates if color depth is 666
  ADL_COLORDEPTH_666 = 1;
  /// Indicates if color depth is 888
  ADL_COLORDEPTH_888 = 2;
  /// Indicates if color depth is 101010
  ADL_COLORDEPTH_101010 = 3;
  /// Indicates if color depth is 121212
  ADL_COLORDEPTH_121212 = 4;
  /// Indicates if color depth is 141414
  ADL_COLORDEPTH_141414 = 5;
  /// Indicates if color depth is 161616
  ADL_COLORDEPTH_161616 = 6;
  /// Indicates default value of color depth
  ADL_COLOR_DEPTH_DEF = ADL_COLORDEPTH_888;
  // @}

  /// \defgroup define_emulation_status
  /// These defines are the status of emulation
  // @{
  /// Indicates if real device is connected.
  ADL_EMUL_STATUS_REAL_DEVICE_CONNECTED = $1;
  /// Indicates if emulated device is presented.
  ADL_EMUL_STATUS_EMULATED_DEVICE_PRESENT = $2;
  /// Indicates if emulated device is used.
  ADL_EMUL_STATUS_EMULATED_DEVICE_USED = $4;
  /// In case when last active real/emulated device used (when persistence is enabled but no emulation enforced then persistence will use last connected/emulated device).
  ADL_EMUL_STATUS_LAST_ACTIVE_DEVICE_USED = $8;
  // @}

  /// \defgroup define_emulation_mode
  /// These defines are the modes of emulation
  // @{
  /// Indicates if no emulation is used
  ADL_EMUL_MODE_OFF = 0;
  /// Indicates if emulation is used when display connected
  ADL_EMUL_MODE_ON_CONNECTED = 1;
  /// Indicates if emulation is used when display dis connected
  ADL_EMUL_MODE_ON_DISCONNECTED = 2;
  /// Indicates if emulation is used always
  ADL_EMUL_MODE_ALWAYS = 3;
  // @}

  /// \defgroup define_emulation_query
  /// These defines are the modes of emulation
  // @{
  /// Indicates Data from real device
  ADL_QUERY_REAL_DATA = 0;
  /// Indicates Emulated data
  ADL_QUERY_EMULATED_DATA = 1;
  /// Indicates Data currently in use
  ADL_QUERY_CURRENT_DATA = 2;
  // @}

  /// \defgroup define_persistence_state
  /// These defines are the states of persistence
  // @{
  /// Indicates persistence is disabled
  ADL_EDID_PERSISTANCE_DISABLED = 0;
  /// Indicates persistence is enabled
  ADL_EDID_PERSISTANCE_ENABLED = 1;
  // @}

  /// \defgroup define_connector_types Connector Type
  /// defines for ADLConnectorInfo.iType
  // @{
  /// Indicates unknown Connector type
  ADL_CONNECTOR_TYPE_UNKNOWN = 0;
  /// Indicates VGA Connector type
  ADL_CONNECTOR_TYPE_VGA = 1;
  /// Indicates DVI-D Connector type
  ADL_CONNECTOR_TYPE_DVI_D = 2;
  /// Indicates DVI-I Connector type
  ADL_CONNECTOR_TYPE_DVI_I = 3;
  /// Indicates Active Dongle-NA Connector type
  ADL_CONNECTOR_TYPE_ATICVDONGLE_NA = 4;
  /// Indicates Active Dongle-JP Connector type
  ADL_CONNECTOR_TYPE_ATICVDONGLE_JP = 5;
  /// Indicates Active Dongle-NONI2C Connector type
  ADL_CONNECTOR_TYPE_ATICVDONGLE_NONI2C = 6;
  /// Indicates Active Dongle-NONI2C-D Connector type
  ADL_CONNECTOR_TYPE_ATICVDONGLE_NONI2C_D = 7;
  /// Indicates HDMI-Type A Connector type
  ADL_CONNECTOR_TYPE_HDMI_TYPE_A = 8;
  /// Indicates HDMI-Type B Connector type
  ADL_CONNECTOR_TYPE_HDMI_TYPE_B = 9;
  /// Indicates Display port Connector type
  ADL_CONNECTOR_TYPE_DISPLAYPORT = 10;
  /// Indicates EDP Connector type
  ADL_CONNECTOR_TYPE_EDP = 11;
  /// Indicates MiniDP Connector type
  ADL_CONNECTOR_TYPE_MINI_DISPLAYPORT = 12;
  /// Indicates Virtual Connector type
  ADL_CONNECTOR_TYPE_VIRTUAL = 13;
  // @}

  /// \defgroup define_freesync_usecase
  /// These defines are to specify use cases in which FreeSync should be enabled
  /// They are a bit mask. To specify FreeSync for more than one use case, the input value
  /// should be set to include multiple bits set
  // @{
  /// Indicates FreeSync is enabled for Static Screen case
  ADL_FREESYNC_USECASE_STATIC = $1;
  /// Indicates FreeSync is enabled for Video use case
  ADL_FREESYNC_USECASE_VIDEO = $2;
  /// Indicates FreeSync is enabled for Gaming use case
  ADL_FREESYNC_USECASE_GAMING = $4;
  // @}

  /// \defgroup define_freesync_caps
  /// These defines are used to retrieve FreeSync display capabilities.
  /// GPU support flag also indicates whether the display is
  /// connected to a GPU that actually supports FreeSync
  // @{
  ADL_FREESYNC_CAP_SUPPORTED = (1 shl 0);
  ADL_FREESYNC_CAP_GPUSUPPORTED = (1 shl 1);
  ADL_FREESYNC_CAP_DISPLAYSUPPORTED = (1 shl 2);
  ADL_FREESYNC_CAP_CURRENTMODESUPPORTED = (1 shl 3);
  ADL_FREESYNC_CAP_NOCFXORCFXSUPPORTED = (1 shl 4);
  ADL_FREESYNC_CAP_NOGENLOCKORGENLOCKSUPPORTED = (1 shl 5);
  ADL_FREESYNC_CAP_BORDERLESSWINDOWSUPPORTED = (1 shl 6);
  // @}

  /// \defgroup define_MST_CommandLine_execute
  // @{
  /// Indicates the MST command line for branch message if the bit is set. Otherwise, it is display message
  ADL_MST_COMMANDLINE_PATH_MSG = $1;
  /// Indicates the MST command line to send message in broadcast way it the bit is set
  ADL_MST_COMMANDLINE_BROADCAST = $2;

  // @}

  /// \defgroup define_Adapter_CloneTypes_Get
  // @{
  /// Indicates there is crossGPU clone with non-AMD dispalys
  ADL_CROSSGPUDISPLAYCLONE_AMD_WITH_NONAMD = $1;
  /// Indicates there is crossGPU clone
  ADL_CROSSGPUDISPLAYCLONE = $2;

// @}

// End Bracket for Constants and Definitions. Add new groups ABOVE this line!

// @}

const
  ADL_PMLOG_MAX_SENSORS = 256;

type
  ADLOD8SettingId = (
    OD8_GFXCLK_FMAX = 0,
    OD8_GFXCLK_FMIN,
    OD8_GFXCLK_FREQ1,
    OD8_GFXCLK_VOLTAGE1,
    OD8_GFXCLK_FREQ2,
    OD8_GFXCLK_VOLTAGE2,
    OD8_GFXCLK_FREQ3,
    OD8_GFXCLK_VOLTAGE3,
    OD8_UCLK_FMAX,
    OD8_POWER_PERCENTAGE,
    OD8_FAN_MIN_SPEED,
    OD8_FAN_ACOUSTIC_LIMIT,
    OD8_FAN_TARGET_TEMP,
    OD8_OPERATING_TEMP_MAX,
    OD8_AC_TIMING,
    OD8_FAN_ZERORPM_CONTROL,
    OD8_AUTO_UV_ENGINE_CONTROL,
    OD8_AUTO_OC_ENGINE_CONTROL,
    OD8_AUTO_OC_MEMORY_CONTROL,
    OD8_FAN_CURVE_TEMPERATURE_1,
    OD8_FAN_CURVE_SPEED_1,
    OD8_FAN_CURVE_TEMPERATURE_2,
    OD8_FAN_CURVE_SPEED_2,
    OD8_FAN_CURVE_TEMPERATURE_3,
    OD8_FAN_CURVE_SPEED_3,
    OD8_FAN_CURVE_TEMPERATURE_4,
    OD8_FAN_CURVE_SPEED_4,
    OD8_FAN_CURVE_TEMPERATURE_5,
    OD8_FAN_CURVE_SPEED_5,
    OD8_WS_FAN_AUTO_FAN_ACOUSTIC_LIMIT,
    RESERVED_1,
    RESERVED_2,
    RESERVED_3,
    RESERVED_4,
    OD8_UCLK_FMIN,
    OD8_FAN_ZERO_RPM_STOP_TEMPERATURE,
    OD8_OPTIMZED_POWER_MODE,
    OD8_OD_VOLTAGE,
    OD8_POWER_GAUGE, //Starting from this is new features with new capabilities and new interface for limits.
    OD8_COUNT
  );

type
  ADLSensorType = (
    SENSOR_MAXTYPES = 0,
    PMLOG_CLK_GFXCLK = 1,
    PMLOG_CLK_MEMCLK = 2,
    PMLOG_CLK_SOCCLK = 3,
    PMLOG_CLK_UVDCLK1 = 4,
    PMLOG_CLK_UVDCLK2 = 5,
    PMLOG_CLK_VCECLK = 6,
    PMLOG_CLK_VCNCLK = 7,
    PMLOG_TEMPERATURE_EDGE = 8,
    PMLOG_TEMPERATURE_MEM = 9,
    PMLOG_TEMPERATURE_VRVDDC = 10,
    PMLOG_TEMPERATURE_VRMVDD = 11,
    PMLOG_TEMPERATURE_LIQUID = 12,
    PMLOG_TEMPERATURE_PLX = 13,
    PMLOG_FAN_RPM = 14,
    PMLOG_FAN_PERCENTAGE = 15,
    PMLOG_SOC_VOLTAGE = 16,
    PMLOG_SOC_POWER = 17,
    PMLOG_SOC_CURRENT = 18,
    PMLOG_INFO_ACTIVITY_GFX = 19,
    PMLOG_INFO_ACTIVITY_MEM = 20,
    PMLOG_GFX_VOLTAGE = 21,
    PMLOG_MEM_VOLTAGE = 22,
    PMLOG_ASIC_POWER = 23,
    PMLOG_TEMPERATURE_VRSOC = 24,
    PMLOG_TEMPERATURE_VRMVDD0 = 25,
    PMLOG_TEMPERATURE_VRMVDD1 = 26,
    PMLOG_TEMPERATURE_HOTSPOT = 27,
    PMLOG_TEMPERATURE_GFX = 28,
    PMLOG_TEMPERATURE_SOC = 29,
    PMLOG_GFX_POWER = 30,
    PMLOG_GFX_CURRENT = 31,
    PMLOG_TEMPERATURE_CPU = 32,
    PMLOG_CPU_POWER = 33,
    PMLOG_CLK_CPUCLK = 34,
    PMLOG_THROTTLER_STATUS = 35,
    PMLOG_CLK_VCN1CLK1 = 36,
    PMLOG_CLK_VCN1CLK2 = 37,
    PMLOG_SMART_POWERSHIFT_CPU = 38,
    PMLOG_SMART_POWERSHIFT_DGPU = 39,
    PMLOG_BUS_SPEED = 40,
    PMLOG_BUS_LANES = 41,
    PMLOG_MAX_SENSORS_REAL
  );


//Throttle Status
type
  ADL_THROTTLE_NOTIFICATION = (
    ADL_PMLOG_THROTTLE_POWER = 1 shl 0,
    ADL_PMLOG_THROTTLE_THERMAL = 1 shl 1,
    ADL_PMLOG_THROTTLE_CURRENT = 1 shl 2
  );

type
  ADL_PMLOG_SENSORS = (
    ADL_SENSOR_MAXTYPES = 0,
    ADL_PMLOG_CLK_GFXCLK = 1,
    ADL_PMLOG_CLK_MEMCLK = 2,
    ADL_PMLOG_CLK_SOCCLK = 3,
    ADL_PMLOG_CLK_UVDCLK1 = 4,
    ADL_PMLOG_CLK_UVDCLK2 = 5,
    ADL_PMLOG_CLK_VCECLK = 6,
    ADL_PMLOG_CLK_VCNCLK = 7,
    ADL_PMLOG_TEMPERATURE_EDGE = 8,
    ADL_PMLOG_TEMPERATURE_MEM = 9,
    ADL_PMLOG_TEMPERATURE_VRVDDC = 10,
    ADL_PMLOG_TEMPERATURE_VRMVDD = 11,
    ADL_PMLOG_TEMPERATURE_LIQUID = 12,
    ADL_PMLOG_TEMPERATURE_PLX = 13,
    ADL_PMLOG_FAN_RPM = 14,
    ADL_PMLOG_FAN_PERCENTAGE = 15,
    ADL_PMLOG_SOC_VOLTAGE = 16,
    ADL_PMLOG_SOC_POWER = 17,
    ADL_PMLOG_SOC_CURRENT = 18,
    ADL_PMLOG_INFO_ACTIVITY_GFX = 19,
    ADL_PMLOG_INFO_ACTIVITY_MEM = 20,
    ADL_PMLOG_GFX_VOLTAGE = 21,
    ADL_PMLOG_MEM_VOLTAGE = 22,
    ADL_PMLOG_ASIC_POWER = 23,
    ADL_PMLOG_TEMPERATURE_VRSOC = 24,
    ADL_PMLOG_TEMPERATURE_VRMVDD0 = 25,
    ADL_PMLOG_TEMPERATURE_VRMVDD1 = 26,
    ADL_PMLOG_TEMPERATURE_HOTSPOT = 27,
    ADL_PMLOG_TEMPERATURE_GFX = 28,
    ADL_PMLOG_TEMPERATURE_SOC = 29,
    ADL_PMLOG_GFX_POWER = 30,
    ADL_PMLOG_GFX_CURRENT = 31,
    ADL_PMLOG_TEMPERATURE_CPU = 32,
    ADL_PMLOG_CPU_POWER = 33,
    ADL_PMLOG_CLK_CPUCLK = 34,
    ADL_PMLOG_THROTTLER_STATUS = 35,
    ADL_PMLOG_CLK_VCN1CLK1 = 36,
    ADL_PMLOG_CLK_VCN1CLK2 = 37,
    ADL_PMLOG_SMART_POWERSHIFT_CPU	= 38,
    ADL_PMLOG_SMART_POWERSHIFT_DGPU = 39,
    ADL_PMLOG_BUS_SPEED = 40,
    ADL_PMLOG_BUS_LANES = 41
  );

implementation

end.
