#ifndef MIDI_H
#define MIDI_H 

#ifdef OSX
#include <CoreMIDI.h>
#endif

/* MIDI message data byte macros, all should be passed a pointer to a byte
 * representing the beginning of a byte array */
#define MIDIMSG_NOTE_GET_PCH(b) ((b)[1])
#define MIDIMSG_NOTE_GET_VEL(b) ((b)[2])

/* MIDI message status byte macros */

#define MIDIMSG_DATA_BYTE_MAX 127 
#define MIDIMSG_NCHANNELS   16 

#define MIDIMSG_NOTE_OFF    0x80
#define MIDIMSG_NOTE_ON     0x90
#define MIDIMSG_POLY_PRS    0xa0
#define MIDIMSG_CNTRL_CHNG  0xb0
#define MIDIMSG_PRGRM_CHNG  0xc0
#define MIDIMSG_CHN_PRS     0xd0
#define MIDIMSG_PCH_BND     0xe0
#define MIDIMSG_SYS_COMMON  0xf0

#define MIDIMSG_IS_STATUS(x) ((x) & 0x80) 
#define MIDIMSG_IS_DATA(x)   (!(MIDIMSG_IS_STATUS(x))) 

#define MIDIMSG_GET_STATUS(x)       ((x) & 0xf0) 
#define MIDIMSG_GET_CHANNEL(x)      ((x) & 0x0f) 
#define MIDIMSG_KNOW_LENGTH(x)      (MIDIMSG_IS_2_BYTE_MSG(x) || MIDIMSG_IS_3_BYTE_MSG(x))  
#define MIDIMSG_GET_LENGTH(x)       (MIDIMSG_IS_2_BYTE_MSG(x) ? 2 : (MIDIMSG_IS_3_BYTE_MSG(x) ? 3 : 0))

#define MIDIMSG_IS_NOTE_OFF(x)      (MIDIMSG_GET_STATUS(x) == MIDIMSG_NOTE_OFF)
#define MIDIMSG_IS_NOTE_ON(x)       (MIDIMSG_GET_STATUS(x) == MIDIMSG_NOTE_ON)
#define MIDIMSG_IS_POLY_PRS(x)      (MIDIMSG_GET_STATUS(x) == MIDIMSG_POLY_PRS)
#define MIDIMSG_IS_CNTRL_CHNG(x)    (MIDIMSG_GET_STATUS(x) == MIDIMSG_CNTRL_CHNG)
#define MIDIMSG_IS_PRGRM_CHNG(x)    (MIDIMSG_GET_STATUS(x) == MIDIMSG_PRGRM_CHNG)
#define MIDIMSG_IS_CHN_PRS(x)       (MIDIMSG_GET_STATUS(x) == MIDIMSG_CHN_PRS)
#define MIDIMSG_IS_PCH_BND(x)       (MIDIMSG_GET_STATUS(x) == MIDIMSG_PCH_BND)

#define MIDIMSG_IS_2_BYTE_MSG(x)    (MIDIMSG_IS_PRGRM_CHNG(x) || \
                                     MIDIMSG_IS_CHN_PRS(x))
#define MIDIMSG_IS_3_BYTE_MSG(x)    (MIDIMSG_IS_NOTE_OFF(x)   || \
                                     MIDIMSG_IS_NOTE_ON(x)    || \
                                     MIDIMSG_IS_POLY_PRS(x)   || \
                                     MIDIMSG_IS_CNTRL_CHNG(x) || \
                                     MIDIMSG_IS_PCH_BND(x))

#ifdef OSX 
typedef MIDIPacket vvvv_midipckt_t;
typedef MIDITimeStamp vvvv_midi_tmstmp_t;
/* Get the message data given a pointer to a midi packet */
#define VVVV_MIDIPCKT_GET_MSG_DATA(p) ((p)->data)
#define VVVV_MIDIPCKT_GET_TIMESTAMP(p) ((p)->timeStamp) 
typedef MIDIPacketList vvvv_midipcktlst_t;
#endif

#define VVVV_MIDIPCKT_NOTE_GET_PCH(p) \
   MIDIMSG_NOTE_GET_PCH(VVVV_MIDIPCKT_GET_MSG_DATA(p)) 

#define VVVV_MIDIPCKT_NOTE_GET_VEL(p) \
   MIDIMSG_NOTE_GET_VEL(VVVV_MIDIPCKT_GET_MSG_DATA(p)) 

#endif /* MIDI_H */
