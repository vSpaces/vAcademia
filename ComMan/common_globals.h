#pragma once

#define COMMAND_SIZE 100
#define DATA_SIZE 800*3
#define BLOCK_SIZE 800*3

#define MAX_GROUP_SIZE 15

enum {OPEN_OUTPUT,PUSH_SB,SB_SYNC,SB_END,SOUND_PACKET,SLUZH_PACKET,NUM_AVATAR, SILENCE_MODE, NEW_CLIENT};

struct STransfer_info
{ int command;
  int id;
  int num_packets;
  LONG data_length; 
  char data[DATA_SIZE]; 
};

struct Sbuffer
{ LONG buffer_len;
  char buffer_val[BLOCK_SIZE];
  WORD id;
  int num_packet;
  int tech_data;
};

struct Sbuffer_info
{ int command;
  union {
		Sbuffer buf;
		int num_avatar;
		};
};

struct SDataAndSize
{ LONG size; 
  char data[DATA_SIZE]; 
};
