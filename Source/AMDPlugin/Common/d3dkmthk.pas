{*******************************************************}
{                                                       }
{       d3dkmthk.pas                                    }
{                                                       }
{       Hackjob translation of the d3dkmthk.h           }
{       Original unit header below                      }
{                                                       }
{       2020-10-05 NaliLord                             }
{                                                       }
{*******************************************************}

(*
/******************************Module*Header**********************************\
*
* Module Name: d3dkmthk.h
*
* Content: longhorn display driver model kernel mode thunk interfaces
*
* Copyright (c) 2003 Microsoft Corporation.  All rights reserved.
\*****************************************************************************/
*)

unit d3dkmthk;

interface

uses
  Winapi.Windows;

const
  D3DKMT_QUERYSTATISTICS_ALLOCATION_PRIORITY_CLASS_MAX = 5;
  D3DKMT_QUERYRESULT_PREEMPTION_ATTEMPT_RESULT_MAX = 16;
  D3DKMT_QUERYSTATISTICS_QUEUE_PACKET_TYPE_MAX = 8;
  D3DKMT_QUERYSTATISTICS_DMA_PACKET_TYPE_MAX = 4;
  D3DKMT_QUERYSTATISTICS_SEGMENT_PREFERENCE_MAX = 5;

type
  D3DKMT_QUERYSTATISTICS_TYPE = (D3DKMT_QUERYSTATISTICS_ADAPTER = 0, D3DKMT_QUERYSTATISTICS_PROCESS = 1, D3DKMT_QUERYSTATISTICS_PROCESS_ADAPTER = 2,
    D3DKMT_QUERYSTATISTICS_SEGMENT = 3, D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT = 4, D3DKMT_QUERYSTATISTICS_NODE = 5, D3DKMT_QUERYSTATISTICS_PROCESS_NODE = 6,
    D3DKMT_QUERYSTATISTICS_VIDPNSOURCE = 7, D3DKMT_QUERYSTATISTICS_PROCESS_VIDPNSOURCE = 8);

  KMTQUERYADAPTERINFOTYPE = (KMTQAITYPE_UMDRIVERPRIVATE = 0, KMTQAITYPE_UMDRIVERNAME = 1, KMTQAITYPE_UMOPENGLINFO = 2, KMTQAITYPE_GETSEGMENTSIZE = 3,
    KMTQAITYPE_ADAPTERGUID = 4, KMTQAITYPE_FLIPQUEUEINFO = 5, KMTQAITYPE_ADAPTERADDRESS = 6, KMTQAITYPE_SETWORKINGSETINFO = 7,
    KMTQAITYPE_ADAPTERREGISTRYINFO = 8, KMTQAITYPE_CURRENTDISPLAYMODE = 9, KMTQAITYPE_MODELIST = 10, KMTQAITYPE_CHECKDRIVERUPDATESTATUS = 11,
    KMTQAITYPE_VIRTUALADDRESSINFO = 12, KMTQAITYPE_DRIVERVERSION = 13, KMTQAITYPE_ADAPTERTYPE = 15, KMTQAITYPE_OUTPUTDUPLCONTEXTSCOUNT = 16,
    KMTQAITYPE_WDDM_1_2_CAPS = 17, KMTQAITYPE_UMD_DRIVER_VERSION = 18, KMTQAITYPE_DIRECTFLIP_SUPPORT = 19);

type
  D3DKMT_QUERYSTATSTICS_REFERENCE_DMA_BUFFER = record
    NbCall: ULONG;
    NbAllocationsReferenced: ULONG;
    MaxNbAllocationsReferenced: ULONG;
    NbNULLReference: ULONG;
    NbWriteReference: ULONG;
    NbRenamedAllocationsReferenced: ULONG;
    NbIterationSearchingRenamedAllocation: ULONG;
    NbLockedAllocationReferenced: ULONG;
    NbAllocationWithValidPrepatchingInfoReferenced: ULONG;
    NbAllocationWithInvalidPrepatchingInfoReferenced: ULONG;
    NbDMABufferSuccessfullyPrePatched: ULONG;
    NbPrimariesReferencesOverflow: ULONG;
    NbAllocationWithNonPreferredResources: ULONG;
    NbAllocationInsertedInMigrationTable: ULONG;
  end;

  D3DKMT_QUERYSTATSTICS_RENAMING = record
    NbAllocationsRenamed: ULONG;
    NbAllocationsShrinked: ULONG;
    NbRenamedBuffer: ULONG;
    MaxRenamingListLength: ULONG;
    NbFailuresDueToRenamingLimit: ULONG;
    NbFailuresDueToCreateAllocation: ULONG;
    NbFailuresDueToOpenAllocation: ULONG;
    NbFailuresDueToLowResource: ULONG;
    NbFailuresDueToNonRetiredLimit: ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_COUNTER = record
    Count: ULONG;
    Bytes: ULONGLONG;
  end;

  D3DKMT_QUERYSTATSTICS_PREPRATION = record
    BroadcastStall: ULONG;
    NbDMAPrepared: ULONG;
    NbDMAPreparedLongPath: ULONG;
    ImmediateHighestPreparationPass: ULONG;
    AllocationsTrimmed: D3DKMT_QUERYSTATISTICS_COUNTER;
  end;

  D3DKMT_QUERYSTATSTICS_PAGING_FAULT = record
    Faults: D3DKMT_QUERYSTATISTICS_COUNTER;
    FaultsFirstTimeAccess: D3DKMT_QUERYSTATISTICS_COUNTER;
    FaultsReclaimed: D3DKMT_QUERYSTATISTICS_COUNTER;
    FaultsMigration: D3DKMT_QUERYSTATISTICS_COUNTER;
    FaultsIncorrectResource: D3DKMT_QUERYSTATISTICS_COUNTER;
    FaultsLostContent: D3DKMT_QUERYSTATISTICS_COUNTER;
    FaultsEvicted: D3DKMT_QUERYSTATISTICS_COUNTER;
    AllocationsMEM_RESET: D3DKMT_QUERYSTATISTICS_COUNTER;
    AllocationsUnresetSuccess: D3DKMT_QUERYSTATISTICS_COUNTER;
    AllocationsUnresetFail: D3DKMT_QUERYSTATISTICS_COUNTER;
    AllocationsUnresetSuccessRead: ULONG;
    AllocationsUnresetFailRead: ULONG;
    Evictions: D3DKMT_QUERYSTATISTICS_COUNTER;
    EvictionsDueToPreparation: D3DKMT_QUERYSTATISTICS_COUNTER;
    EvictionsDueToLock: D3DKMT_QUERYSTATISTICS_COUNTER;
    EvictionsDueToClose: D3DKMT_QUERYSTATISTICS_COUNTER;
    EvictionsDueToPurge: D3DKMT_QUERYSTATISTICS_COUNTER;
    EvictionsDueToSuspendCPUAccess: D3DKMT_QUERYSTATISTICS_COUNTER;
  end;

  D3DKMT_QUERYSTATSTICS_PAGING_TRANSFER = record
    BytesFilled: ULONGLONG;
    BytesDiscarded: ULONGLONG;
    BytesMappedIntoAperture: ULONGLONG;
    BytesUnmappedFromAperture: ULONGLONG;
    BytesTransferredFromMdlToMemory: ULONGLONG;
    BytesTransferredFromMemoryToMdl: ULONGLONG;
    BytesTransferredFromApertureToMemory: ULONGLONG;
    BytesTransferredFromMemoryToAperture: ULONGLONG;
  end;

  D3DKMT_QUERYSTATSTICS_SWIZZLING_RANGE = record
    NbRangesAcquired: ULONG;
    NbRangesReleased: ULONG;
  end;

  D3DKMT_QUERYSTATSTICS_LOCKS = record
    NbLocks: ULONG;
    NbLocksWaitFlag: ULONG;
    NbLocksDiscardFlag: ULONG;
    NbLocksNoOverwrite: ULONG;
    NbLocksNoReadSync: ULONG;
    NbLocksLinearization: ULONG;
    NbComplexLocks: ULONG;
  end;

  D3DKMT_QUERYSTATSTICS_ALLOCATIONS = record
    Created: D3DKMT_QUERYSTATISTICS_COUNTER;
    Destroyed: D3DKMT_QUERYSTATISTICS_COUNTER;
    Opened: D3DKMT_QUERYSTATISTICS_COUNTER;
    Closed: D3DKMT_QUERYSTATISTICS_COUNTER;
    MigratedSuccess: D3DKMT_QUERYSTATISTICS_COUNTER;
    MigratedFail: D3DKMT_QUERYSTATISTICS_COUNTER;
    MigratedAbandoned: D3DKMT_QUERYSTATISTICS_COUNTER;
  end;

  D3DKMT_QUERYSTATSTICS_TERMINATIONS = record
    TerminatedShared: D3DKMT_QUERYSTATISTICS_COUNTER;
    TerminatedNonShared: D3DKMT_QUERYSTATISTICS_COUNTER;
    DestroyedShared: D3DKMT_QUERYSTATISTICS_COUNTER;
    DestroyedNonShared: D3DKMT_QUERYSTATISTICS_COUNTER;
  end;

  D3DKMT_QUERYSTATISTICS_ADAPTER_INFORMATION = record
    NbSegments: ULONG;
    NodeCount: ULONG;
    VidPnSourceCount: ULONG;
    VSyncEnabled: ULONG;
    TdrDetectedCount: ULONG;
    ZeroLengthDmaBuffers: LONGLONG;
    RestartedPeriod: ULONGLONG;
    ReferenceDmaBuffer: D3DKMT_QUERYSTATSTICS_REFERENCE_DMA_BUFFER;
    Renaming: D3DKMT_QUERYSTATSTICS_RENAMING;
    Preparation: D3DKMT_QUERYSTATSTICS_PREPRATION;
    PagingFault: D3DKMT_QUERYSTATSTICS_PAGING_FAULT;
    PagingTransfer: D3DKMT_QUERYSTATSTICS_PAGING_TRANSFER;
    SwizzlingRange: D3DKMT_QUERYSTATSTICS_SWIZZLING_RANGE;
    Locks: D3DKMT_QUERYSTATSTICS_LOCKS;
    Allocations: D3DKMT_QUERYSTATSTICS_ALLOCATIONS;
    Terminations: D3DKMT_QUERYSTATSTICS_TERMINATIONS;
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_MEMORY = record
    TotalBytesEvicted: ULONGLONG;
    AllocsCommitted: ULONG;
    AllocsResident: ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_SEGMENT_INFORMATION = record
    CommitLimit: ULONGLONG;
    BytesCommitted: ULONGLONG;
    BytesResident: ULONGLONG;
    Memory: D3DKMT_QUERYSTATISTICS_MEMORY;
    Aperture: ULONG;
    TotalBytesEvictedByPriority: Array[0..D3DKMT_QUERYSTATISTICS_ALLOCATION_PRIORITY_CLASS_MAX-1] of ULONGLONG;   //Size = D3DKMT_MaxAllocationPriorityClass
    SystemMemoryEndAddress: UINT64;
    PowerFlags: record
      PreservedDuringStandby: UINT64;
      PreservedDuringHibernate: UINT64;
      PartiallyPreservedDuringHibernate: UINT64;
      Reserved: UINT64;
    end;
    Reserved: Array[0..5] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_PREEMPTION_INFORMATION = record
    PreemptionCounter: Array[0..D3DKMT_QUERYRESULT_PREEMPTION_ATTEMPT_RESULT_MAX-1] of ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_QUEUE_PACKET_TYPE_INFORMATION = record
    PacketSubmited: ULONG;
    PacketCompleted: ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_DMA_PACKET_TYPE_INFORMATION = record
    PacketSubmited: ULONG;
    PacketCompleted: ULONG;
    PacketPreempted: ULONG;
    PacketFaulted: ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_PACKET_INFORMATION = record
    QueuePacket: Array[0..D3DKMT_QUERYSTATISTICS_QUEUE_PACKET_TYPE_MAX-1] of D3DKMT_QUERYSTATISTICS_QUEUE_PACKET_TYPE_INFORMATION;  //Size = D3DKMT_QueuePacketTypeMax
    DmaPacket: Array[0..D3DKMT_QUERYSTATISTICS_DMA_PACKET_TYPE_MAX-1] of D3DKMT_QUERYSTATISTICS_DMA_PACKET_TYPE_INFORMATION;    //Size = D3DKMT_DmaPacketTypeMax
  end;

  D3DKMT_QUERYSTATISTICS_PROCESS_NODE_INFORMATION = record
    RunningTime: LARGE_INTEGER;
    ContextSwitch: ULONG;
    PreemptionStatistics: D3DKMT_QUERYSTATISTICS_PREEMPTION_INFORMATION;
    PacketStatistics: D3DKMT_QUERYSTATISTICS_PACKET_INFORMATION;
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_NODE_INFORMATION = record
    GlobalInformation: D3DKMT_QUERYSTATISTICS_PROCESS_NODE_INFORMATION;   //Global statistics
    SystemInformation: D3DKMT_QUERYSTATISTICS_PROCESS_NODE_INFORMATION;   //Statistics for system thread
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_PROCESS_VIDPNSOURCE_INFORMATION = record
    Frame: ULONG;            // both by Blt and Flip.
    CancelledFrame: ULONG;   // by restart (flip only).
    QueuedPresent: ULONG;    // queued present.
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_VIDPNSOURCE_INFORMATION = record
    GlobalInformation: D3DKMT_QUERYSTATISTICS_PROCESS_VIDPNSOURCE_INFORMATION;     //Global statistics
    SystemInformation: D3DKMT_QUERYSTATISTICS_PROCESS_VIDPNSOURCE_INFORMATION;     //Statistics for system thread
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_SYSTEM_MEMORY = record
    BytesAllocated: ULONGLONG;
    BytesReserved: ULONGLONG;
    SmallAllocationBlocks: ULONG;
    LargeAllocationBlocks: ULONG;
    WriteCombinedBytesAllocated: ULONGLONG;
    WriteCombinedBytesReserved: ULONGLONG;
    CachedBytesAllocated: ULONGLONG;
    CachedBytesReserved: ULONGLONG;
    SectionBytesAllocated: ULONGLONG;
    SectionBytesReserved: ULONGLONG;
  end;

  D3DKMT_QUERYSTATISTICS_PROCESS_INFORMATION = record
    NodeCount: ULONG;
    VidPnSourceCount: ULONG;
    SystemMemory: D3DKMT_QUERYSTATISTICS_SYSTEM_MEMORY;
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_DMA_BUFFER = record
    Size: D3DKMT_QUERYSTATISTICS_COUNTER;
    AllocationListBytes: ULONG;
    PatchLocationListBytes: ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_COMMITMENT_DATA = record
    TotalBytesEvictedFromProcess: UINT64;
    BytesBySegmentPreference: Array[0..D3DKMT_QUERYSTATISTICS_SEGMENT_PREFERENCE_MAX-1] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_POLICY = record
    PreferApertureForRead: Array[0..D3DKMT_QUERYSTATISTICS_ALLOCATION_PRIORITY_CLASS_MAX-1] of ULONGLONG;
    PreferAperture: Array[0..D3DKMT_QUERYSTATISTICS_ALLOCATION_PRIORITY_CLASS_MAX-1] of ULONGLONG;
    MemResetOnPaging: ULONGLONG;
    RemovePagesFromWorkingSetOnPaging: ULONGLONG;
    MigrationEnabled: ULONGLONG;
  end;

  D3DKMT_QUERYSTATISTICS_PROCESS_ADAPTER_INFORMATION = record
    NbSegments: ULONG;
    NodeCount: ULONG;
    VidPnSourceCount: ULONG;
    VirtualMemoryUsage: ULONG;
    DmaBuffer: D3DKMT_QUERYSTATISTICS_DMA_BUFFER;
    CommitmentData: D3DKMT_QUERYSTATISTICS_COMMITMENT_DATA;
    Policy: D3DKMT_QUERYSTATISTICS_POLICY;
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_VIDEO_MEMORY = record
    AllocsCommitted: ULONG;
    AllocsResidentInP: Array[0..D3DKMT_QUERYSTATISTICS_SEGMENT_PREFERENCE_MAX-1] of D3DKMT_QUERYSTATISTICS_COUNTER;
    AllocsResidentInNonPreferred: D3DKMT_QUERYSTATISTICS_COUNTER;
    TotalBytesEvictedDueToPreparation: ULONGLONG;
  end;

  D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_POLICY = record
    UseMRU: ULONGLONG;
  end;

  D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_INFORMATION = record
    BytesCommitted: ULONGLONG;
    MaximumWorkingSet: ULONGLONG;
    MinimumWorkingSet: ULONGLONG;
    NbReferencedAllocationEvictedInPeriod: ULONG;
    VideoMemory: D3DKMT_QUERYSTATISTICS_VIDEO_MEMORY;
    Policy: D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_POLICY;
    Reserved: Array[0..7] of UINT64;
  end;

  D3DKMT_QUERYSTATISTICS_RESULT = record
    case Integer of
      0:(AdapterInformation: D3DKMT_QUERYSTATISTICS_ADAPTER_INFORMATION);                            // out: result of D3DKMT_QUERYSTATISTICS_ADAPTER query
      1:(SegmentInformation: D3DKMT_QUERYSTATISTICS_SEGMENT_INFORMATION);                            // out: result of D3DKMT_QUERYSTATISTICS_SEGMENT query
      2:(NodeInformation: D3DKMT_QUERYSTATISTICS_NODE_INFORMATION);                                  // out: result of D3DKMT_QUERYSTATISTICS_NODE query
      3:(VidPnSourceInformation: D3DKMT_QUERYSTATISTICS_VIDPNSOURCE_INFORMATION);                    // out: result of D3DKMT_QUERYSTATISTICS_VIDPNSOURCE query
      4:(ProcessInformation: D3DKMT_QUERYSTATISTICS_PROCESS_INFORMATION);                            // out: result of D3DKMT_QUERYSTATISTICS_PROCESS query
      5:(ProcessAdapterInformation: D3DKMT_QUERYSTATISTICS_PROCESS_ADAPTER_INFORMATION);             // out: result of D3DKMT_QUERYSTATISTICS_PROCESS_ADAPTER query
      6:(ProcessSegmentInformation: D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT_INFORMATION);             // out: result of D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT query
      7:(ProcessNodeInformation: D3DKMT_QUERYSTATISTICS_PROCESS_NODE_INFORMATION);                   // out: result of D3DKMT_QUERYSTATISTICS_PROCESS_NODE query
      8:(ProcessVidPnSourceInformation: D3DKMT_QUERYSTATISTICS_PROCESS_VIDPNSOURCE_INFORMATION);     // out: result of D3DKMT_QUERYSTATISTICS_PROCESS_VIDPNSOURCE query
  end;

  D3DKMT_QUERYSTATISTICS_QUERY_SEGMENT = record
    SegmentId: ULONG;   // in: id of node to get statistics for
  end;

  D3DKMT_QUERYSTATISTICS_QUERY_NODE = record
    NodeId: ULONG;
  end;

  D3DKMT_QUERYSTATISTICS_QUERY_VIDPNSOURCE = record
    VidPnSourceId: ULONG;   // in: id of segment to get statistics for
  end;

  D3DKMT_QUERYSTATISTICS = record
    Typ: D3DKMT_QUERYSTATISTICS_TYPE;          // in: aType of data requested
    AdapterLuid: LUID;   // in: adapter to get export / statistics from
    hProcess: THandle;      // in: process to get statistics for, if required for this query type
    QueryResult: D3DKMT_QUERYSTATISTICS_RESULT;   // out: requested data
    case Integer of
      0:(QuerySegment: D3DKMT_QUERYSTATISTICS_QUERY_SEGMENT);
      1:(QueryProcessSegment: D3DKMT_QUERYSTATISTICS_QUERY_SEGMENT);
      2:(QueryNode: D3DKMT_QUERYSTATISTICS_QUERY_NODE);
      3:(QueryProcessNode: D3DKMT_QUERYSTATISTICS_QUERY_NODE);
      4:(QueryVidPnSource: D3DKMT_QUERYSTATISTICS_QUERY_VIDPNSOURCE);
      5:(QueryProcessVidPnSource: D3DKMT_QUERYSTATISTICS_QUERY_VIDPNSOURCE);
  end;

  D3DKMT_QUERYADAPTERINFO = record
    hAdapter: ULONG;
    aType: KMTQUERYADAPTERINFOTYPE;
    pPrivateDriverData: Pointer;
    PrivateDriverDataSize: UINT;
  end;

  D3DKMT_OPENADAPTERFROMDEVICENAME = record
    pDeviceName: PWideChar;      // in:  NULL terminated string containing the device name to open
    hAdapter: ULONG;         // out: adapter handle
    AdapterLuid: LUID;      // out: adapter LUID
  end;

  D3DKMT_OPENADAPTERFROMHDC = record
    hDc: HDC;              // in:  DC that maps to a single display
    hAdapter: ULONG;         // out: adapter handle
    AdapterLuid: LUID;      // out: adapter LUID
    VidPnSourceId: UINT;    // out: VidPN source ID for that particular display
  end;

  D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME = record
    DeviceName: Array[0..31] of Char; // in:  Name of GDI device from which to open an adapter instance
    hAdapter: ULONG;         // out: adapter handle
    AdapterLuid: LUID;      // out: adapter LUID
    VidPnSourceId: UINT;    // out: VidPN source ID for that particular display
  end;

  D3DKMT_CLOSEADAPTER = record
    hAdapter: ULONG;     // in: adapter handle
  end;

  D3DKMT_SEGMENTSIZEINFO = record
    DedicatedVideoMemorySize: ULONGLONG; // The size, in bytes, of memory that is dedicated from video memory.
    DedicatedSystemMemorySize: ULONGLONG; // The size, in bytes, of memory that is dedicated from system memory.
    SharedSystemMemorySize: ULONGLONG; // The size, in bytes, of memory from system memory that can be shared by many users.
  end;

function D3DKMTQueryAdapterInfo(var Adapter: D3DKMT_QUERYADAPTERINFO): HRESULT; stdcall; external 'gdi32.dll';
function D3DKMTOpenAdapterFromHdc(var Adapter: D3DKMT_OPENADAPTERFROMHDC): HRESULT; stdcall; external 'gdi32.dll';
function D3DKMTOpenAdapterFromGdiDisplayName(var Adapter: D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME): HRESULT; stdcall; external 'gdi32.dll';
function D3DKMTOpenAdapterFromDeviceName(var Adapter: D3DKMT_OPENADAPTERFROMDEVICENAME): HRESULT; stdcall; external 'gdi32.dll';
function D3DKMTCloseAdapter(var Adapter: D3DKMT_CLOSEADAPTER): HRESULT; stdcall; external 'gdi32.dll';
function D3DKMTQueryStatistics(var Statistics: D3DKMT_QUERYSTATISTICS): HRESULT; stdcall; external 'gdi32.dll';

implementation

end.
