#include "command_parser.h"
#include "../file_operations/header_files/lfs.h"
#include <string.h>
#include "../file_operations/header_files/lfs_util.h"

const uint8_t NACK[7] = {0x53,0xac, 1,2,3, 0x7e};
const uint8_t ACK[7] = {0x53,0xac, 1,2,3, 0x7e};
uint8_t digipeating;

void parser(){
	myprintf("parser called");
}

void create_empty_files(){

	  int fd;
	  int i =0;
	  char file_names[][100]={"strHealth.txt\0", "cam.txt\0", "test.txt\0", "adcs.txt\0", "flags.txt\0"};
//	  for(int i =0 ; i<sizeof(file_names);i++){
//		      mymyprintf("%s", file_names[i]);
		      fd = lfs_file_open(&lfs, &file, "strHealth.txt", LFS_O_CREAT);
		 	  if (fd >= 0)
		 	  {
		 	    myprintf( "File named %s has been created successfully.\n", file_names[i]);
		 	  }
		 	  else
		 	  {
		 	    myprintf("Error opening file: %s\n", file_names[i]);
		 	  }
		 	  lfs_file_close(&lfs, &file);
//	  }
}
/*
@brief :
The function performs truncate operation
*/
void truncate_text_file( struct FILE_OPERATIONS *file_operations, lfs_t lfs, lfs_file_t file)
{

  int fd = lfs_file_open(&lfs, &file, file_operations->filepath, LFS_O_TRUNC | LFS_O_CREAT);
  if (fd >= 0)
  {
    myprintf( "File named %s has been truncated successfully.\n", file_operations->filepath);
  }
  else
  {
    myprintf("Error opening file: %s\n", file_operations->filepath);
  }
  lfs_file_close(&lfs, &file);
}
void truncate_text_file1(lfs_t Lfs, lfs_file_t File)
{

  int fd;
  fd = lfs_file_open(&Lfs, &File, "flags.txt", LFS_O_TRUNC );// |LFS_O_APPEND |LFS_O_APPEND
  		  lfs_file_close(&Lfs, &File);

}



void track_read_seek_pointer(struct FILE_OPERATIONS *file_pointer, int8_t seek_pointer[16])
{
  myprintf("track read seek pointer called \n");
  // SEEK_POINTER seek_pointer;
   int index = 0;
   lfs_t lfs, lfs2;
   lfs_file_t file, file2;
   int fd;
   fd = lfs_file_open(&lfs, &file, file_pointer->filepath, LFS_O_RDONLY);
   uint32_t address = file_pointer->address[3] << 24 | file_pointer->address[2] << 16 | file_pointer->address[1] << 8 | file_pointer->address[0] & 0xff;

  // if (readBytes > 0)
  {
    if (file_pointer->filepath == "satHealth.txt")
      index = 0;
    else if (file_pointer->filepath == "cam.txt")
      index = 4;

    else if (file_pointer->filepath == "epdm.txt")
      index = 4 * 2;

    else if (file_pointer->filepath == "adcs.txt")
      index = 3 * 4;

    else
      myprintf( "Some error while updating flags data\n");
  }
  seek_pointer[index] = file_pointer->address[0];
  seek_pointer[index + 1] = file_pointer->address[1];
  seek_pointer[index + 2] = file_pointer->address[2];
  seek_pointer[index + 3] = file_pointer->address[3];
  ssize_t writeBytes = lfs_file_write(&lfs, &file , seek_pointer, sizeof(seek_pointer));
  if (writeBytes > 0)
  {
    myprintf( "Updated seek pointer data saved to mfm\n");
    lfs_file_close(&lfs, &file);
    fd = lfs_file_open(&lfs, &file, file_pointer->filepath, LFS_O_CREAT | LFS_O_WRONLY);
    if (fd > 0)
    {
      writeBytes = lfs_file_write(&lfs, &file, seek_pointer, sizeof(seek_pointer));
      if (writeBytes > 0)
      {
        myprintf( "updated seek pointer data to sfm\n");
      }
    }
  }
  lfs_file_close(&lfs,&file);
}

void download_file_from_flash(struct FILE_OPERATIONS *file_operations, uint8_t *data_retrieved, uint8_t size_of_buffer,  lfs_t lfs, lfs_file_t file)
{

  int fd;
  int8_t seek_pointer[16];

  uint8_t update_address = 0;
  uint32_t address = file_operations->address[3] << 24 | file_operations->address[2] << 16 | file_operations->address[1] << 8 | file_operations->address[0] & 0xff;
  uint16_t number_of_packets = file_operations->number_of_packets[1] << 8 | file_operations->number_of_packets[0] & 0xff;
  ssize_t read_bytes;
  if (address == 0)
     {
       update_address = 1;
       // todo read the address from the text file
       lfs_file_t fl1;
       int fd2 =  lfs_file_open(&lfs, &file, "seek_pointer.txt", LFS_O_CREAT | LFS_O_RDONLY);
       if (fd2 >= 0)
       {
         ssize_t readBytes2 = lfs_file_read(&lfs, &file, seek_pointer, sizeof(seek_pointer));
         if (readBytes2 < 0)
         {
           myprintf( "Error while reading the seek_pointer.txt in mfm\n");
          lfs_file_close(&lfs, &file);
         }
       }
       else{
    	   myprintf( "Error while opening the seek_pointer.txt in mfm\n");
		   lfs_file_close(&lfs, &file);
       }
     }
  fd = lfs_file_open(&lfs, &file, file_operations->filepath, LFS_O_RDONLY);
  if (fd < 0)
  {
    myprintf( "File named %s reading mode failed fd:%d\n", file_operations->filepath, fd);
    return;
  }
  else
  {

    int size_of_file = lfs_file_seek(&lfs, &file, 0, LFS_SEEK_END);
    int off; //= lfs_file_seek(&lfs, &file, address, SEEK_SET);
    myprintf("\nSize of file is %d %d\n", size_of_file, address);
    int loop1 = 0;
    do
    {
      if (size_of_file > 0 & size_of_file > address)
      {
        off = lfs_file_seek(&lfs, &file, address, LFS_SEEK_SET); // Set file pointer to the calculated address
        read_bytes = lfs_file_read(&lfs, &file, data_retrieved, size_of_buffer);
        if (read_bytes >= 0)
        {
          myprintf( "Data retrieved from the flash address %d\n", address);

          myprintf("\n--------------------**************Read size = %zd\n", read_bytes);
          myprintf("\n\n--------------------------Data received----\n");
          for (int j = 0; j < size_of_buffer; j++)
          {
            myprintf("%02x|%c ", data_retrieved[j], data_retrieved[j]); // Print in hexadecimal format
          }
          if (number_of_packets > 0)
            number_of_packets -= 1;
          loop1 += 1;
          address += size_of_buffer;
//          lfs_file_close(&lfs,&file);
        }
        else
        {
          myprintf( "Failed to read data from the flash address %d\n", address);
          lfs_file_close(&lfs,&file);
          break;
        }
      }
    } while (number_of_packets > 1); // loop1 < number_of_packets |
    // todo : add seekpointer read index in internal and external flash memories
    if (update_address == 1)
    {
      file_operations->address[3] = (uint8_t)address >> 24 & 0xff;
      file_operations->address[2] = (uint8_t)address >> 16 & 0xff;
      file_operations->address[1] = (uint8_t)address >> 8 & 0xff;
      file_operations->address[0] = (uint8_t)address & 0xff;
      track_read_seek_pointer(file_operations, seek_pointer);
    }
  }
  lfs_file_close(&lfs, &file);
  // data_retrieved[read_bytes]= '\0';
  // myprintf("\n\n--------------------------Data received----\n");
  // for (int j = 0; j < sizeof(data_retrieved); j++)
  // {
  //   myprintf("%02x|%c ", data_retrieved[j],data_retrieved1[j]); // Print in hexadecimal format
  // }
  myprintf("\n--------------------**************Size = %zu\n", sizeof(data_retrieved));
}

/*
@brief:
This function is used to perform file operation and is called only if the command has the same number as that of OBC_MCU
Two cases:
  1.if the command is 0xca then perform truncate operation
  2.if the main command is 0x1d then perform download operation i.e. read from flash
@params
file_operations: the file_operations pointer variable holds the data required to perform file operations

@struct parameters:
cmd: main command,
select_file: enum to select the required text file,
select_flash: enum to select the required partition to read the text file,
rsv_table: this contains the information how lately this command is to be executed i.e. time latency info
filepath: the full path to the textfile: MOUNT_POINT/TEXT_FILENAME
address: 4 byte of adress data here it might be the data download counter
number of packets: may be DNC(0x00,0x00) or number of packets in 2 bytes.
*/
void perform_file_operations(struct FILE_OPERATIONS *file_operations, lfs_t Lfs, lfs_file_t File)
{
  int data_retrieved[122] = {'\0'};
  myprintf("*********************perform file operations has been called******\n");

  switch (file_operations->cmd)
  {
  case 0xca:
    truncate_text_file(file_operations, Lfs, File);
    myprintf("-----Trucate text file called \n");
    break;
  case 0x1d:
    download_file_from_flash(file_operations, data_retrieved, 112, Lfs, File);
    myprintf("*******************\nsize:%d\n***********************\ncmd : %d, select_file:%d, select_flash: %d, rsv_table:%d, filepath:%s,address :%d %d %d %d, number_of packets:%d %d\n",
           sizeof(data_retrieved), file_operations->cmd, file_operations->select_flash, file_operations->select_file, file_operations->rsv_table[1], file_operations->rsv_table[0], file_operations->filepath,
           file_operations->address[3], file_operations->address[2], file_operations->address[1], file_operations->address[0],
           file_operations->number_of_packets[0], file_operations->number_of_packets[1]);

    myprintf("-------Data download function has been called\n");

    break;
  default:
    break;
  }
  myprintf("\n**************----\nhey filepath is %s\n***********\n", file_operations->filepath);
}


void parse_command(uint8_t COM_RX_DATA[30], lfs_t Lfs1, lfs_file_t File1)
{
  myprintf("parse command starting\n");
  for (int j = 0; j < 25; j++)
  {
    myprintf("%02x|%c ,", COM_RX_DATA[j], COM_RX_DATA[j]);
  }

  uint8_t HEADER = 0, MCU_ID = 0;
  if (COM_RX_DATA[16] >= 0x01 && COM_RX_DATA[16] <= 0x05)
  {
    HEADER = 16;
    MCU_ID = COM_RX_DATA[HEADER];
  }
  else if (COM_RX_DATA[17] >= 0x01 && COM_RX_DATA[17] <= 0x05)
  {
    HEADER = 17;
    MCU_ID = COM_RX_DATA[HEADER];
  }

  myprintf("**********\n*************\nHere the com rx data is %02x,%02x,%02x,%02x\n********************\n********************\n",
         HEADER, COM_RX_DATA[HEADER], COM_RX_DATA[HEADER + 1], COM_RX_DATA[HEADER + 2]);
  uint8_t ack[85] = {0x53, 0xac, 0x04, 0x01, 0x62, 0x63, 0x7e};
  ack[83] = 0x7e;
  ack[82] = 0x7e;

  uint8_t cmds[3];

  cmds[0] = (uint8_t)COM_RX_DATA[HEADER + 1];
  cmds[1] = (uint8_t)COM_RX_DATA[HEADER + 2];
  cmds[2] = (uint8_t)COM_RX_DATA[HEADER + 3];
  myprintf("**********\n*************\nHere the com rx data is %02x,%02x,%02x,%02x\n********************\n********************\n",
         HEADER, cmds[0], cmds[0 + 1], cmds[0 + 2]);

  if (cmds[0] == 0xfd & cmds[1] == 0xba & cmds[2] == 0xd0)
  {
    // HAL_GPIO_WritePin(GPIO_COM_4V_EN_PORT,GPIO_COM_4V_EN_PIN, GPIO_PIN_SET);
    myprintf("\n ********************Digipeater mode turned on********************\n");
    digipeating = 1;
    // //sleep(3);
    // HAL_GPIO_WritePin(GPIO_COM_4V_EN_PORT,GPIO_COM_4V_EN_PIN, GPIO_PIN_RESET);

    return 33;
  }
  else if (cmds[0] == 0xdf & cmds[1] == 0xab & cmds[2] == 0xd1)
  {
    myprintf("\n ********************Digipeater mode turned off********************\n");
    digipeating = 0;
    return 33;
  }
  myprintf("MCU ID %d |%02x\n", cmds[0]);
  switch (MCU_ID)
  {
  case OBC_MCU: /*
                Command to perform flash operations by the OBC
                */

  { // __file_operations.cmd = COM_RX_DATA[HEADER + 1];
    myprintf("OBC MCU ID has been received\n");
    // break; // TO remove this later
    if (cmds[0] == 0xCA | cmds[0] == 0x1D)
    {
      struct FILE_OPERATIONS __file_operations = {
          .cmd = 0x00,
          .number_of_packets = {0}, // Initialize all elements to 0
          .filepath = {'\0'},       // Initialize as an empty string
          .address = {0},           // Initialize all elements to 0
          .rsv_table = {0}          // Initialize all elements to 0
      };

      __file_operations.cmd = cmds[0];
      // __file_operations.select_file = ;
      if ((COM_RX_DATA[HEADER + 2] == 0xD1) || (COM_RX_DATA[HEADER + 2] == 0xD2))
      {
        __file_operations.select_flash = MAIN_FLASH_MEMORY;
//        if ((COM_RX_DATA[HEADER + 2] == 0xD2))
//          strcpy(__file_operations.filepath, MFM_MSN_STRPATH);
//        else
//          strcpy(__file_operations.filepath, MFM_MAIN_STRPATH);
      }
      if ((COM_RX_DATA[HEADER + 2] == 0xD3) || (COM_RX_DATA[HEADER + 2] == 0xD4))
      {
        __file_operations.select_flash = SHARED_FLASH_MEMORY;
//        if ((COM_RX_DATA[HEADER + 2] == 0xD3))
//          strcpy(__file_operations.filepath, SFM_MAIN_STRPATH);
//        else
//          strcpy(__file_operations.filepath, SFM_MSN_STRPATH);
      }
      char filename[7][30] = {"flags.txt", "satHealth.txt", "satellite_Logs.txt", "reservation_table.txt", "cam.txt", "epdm.txt", "adcs.txt"};
      myprintf("Cmd[0]: %02x Cmd[1]: %02x Cmd[2]: %02x\n",cmds[0],cmds[1],cmds[2]);
      if ((cmds[2] == 0xF1))
      {
        __file_operations.select_file = FLAGS;
        strcat(__file_operations.filepath, "flags.txt");
        myprintf("Selected file is %s\n", __file_operations.select_file);
      }
      else if ((cmds[2] == 0xF2))
      {
        __file_operations.select_file = SATELLITE_HEALTH;
        strcat(__file_operations.filepath, "satHealth.txt");
        myprintf("Selected file is %s\n", __file_operations.select_file);
      }
      else if ((cmds[2] == 0xF3))
      {
        __file_operations.select_file = SATELLITE_LOG;
        strcat(__file_operations.filepath, "satHealth.txt");
      }
      else if ((cmds[2] == 0xF4))
      {
        __file_operations.select_file = RESERVATION_TABLE;
        strcat(__file_operations.filepath, "reservation_table.txt");
      }
      else if ((cmds[2] == 0xF5))
      {
        strcat(__file_operations.filepath, "cam.txt");

        __file_operations.select_file = CAMERA_TXT;
      }
      else if ((cmds[2] == 0xF6))
      {
        __file_operations.select_file = EPDM_TXT;
        strcat(__file_operations.filepath, "epdm.txt");
      }
      else if ((cmds[2] == 0xF7))
      {
        __file_operations.select_file = ADCS_TXT;
        strcat(__file_operations.filepath, "adcs.txt");
      }

      __file_operations.rsv_table[0] = COM_RX_DATA[HEADER + 4];
      __file_operations.rsv_table[1] = COM_RX_DATA[HEADER + 5];

      __file_operations.address[0] = COM_RX_DATA[HEADER + 6];
      __file_operations.address[1] = COM_RX_DATA[HEADER + 7];
      __file_operations.address[2] = COM_RX_DATA[HEADER + 8];
      __file_operations.address[3] = COM_RX_DATA[HEADER + 9];
      // myprintf("cmd : %d, select_file:%d, select_flash: %d, rsv_table:%d, filepath:%s,address :%d %d %d %d, number_of packets:%d %d\n",
      //    __file_operations.cmd, __file_operations.select_flash, __file_operations.select_file, __file_operations.rsv_table[1], __file_operations.rsv_table[0], __file_operations.filepath,
      //    __file_operations.address[3], __file_operations.address[2], __file_operations.address[1], __file_operations.address[0],
      //    __file_operations.number_of_packets[0], __file_operations.number_of_packets[1]);
      if (HAL_UART_Transmit(&COM_uart, ack, sizeof(ack), 1000 ) == HAL_OK){myprintf("Acknowledge sent successfully\n");}
      perform_file_operations(&__file_operations, Lfs1, File1);
     if (cmds[0] == 0x1e)//can be used inside perform_file_operation looking at values of file_operation.select_file
      {
        int x, ack[85], fd, ret;
        myprintf("Data download command received\n"); // if reservation command is received then store the reservation command (do not execute)

        ack[0] = 0x53;
        // ack[1] = 0x0e;
        if (cmds[2] == 0xf6)
        {
          ack[1] = 0x0b;
        }
        else if (cmds[2] == 0xf7)
        {
          ack[1] = 0x0d;
        }
        else if (cmds[2] == 0xf5)
        {
          ack[1] = 0x0c;
        }

        ack[2] = 0x51;
        for (int i = 3; i < 83; i++)
        {
          ack[i] = i;
        }
        ack[83] = 0x7e;
        int j;
        // //sleep(2);

        for (j = 0; j < 10; j++)
        {
          myprintf("\n  Data download packet no %d\n", j + 1);
          ack[82] = j + 1;
          ack[81] = j + 1;


         HAL_GPIO_WritePin(GPIO_COM_4V_EN_PORT, GPIO_COM_4V_EN_PIN, GPIO_PIN_SET);
          myprintf("Turning on 4V dcdc  line..\n");
//          gpio_write(GPIO_DCDC_4V_EN, 1);
          HAL_GPIO_WritePin(GPIO_DCDC_4V_EN_PORT, GPIO_DCDC_4V_EN_PIN, GPIO_PIN_SET);

          myprintf("Turning on 4v RF line..\n");
          //sleep(1);
          ret = write(fd, ack, BEACON_DATA_SIZE);
          x = 0;
//          for (int i = 0; i < 85; i++)
//          {
//            myprintf("%02x ", ack[i]);
//          }
         //close(fd);
          myprintf("Turning off 4v RF line..\n");
          HAL_GPIO_WritePin(GPIO_COM_4V_EN_PORT,GPIO_COM_4V_EN_PIN, GPIO_PIN_RESET);
          myprintf("Turning off 4v dcdc EN line..\n");
          HAL_GPIO_WritePin(GPIO_DCDC_4V_EN_PORT, GPIO_DCDC_4V_EN_PIN, GPIO_PIN_RESET);
          myprintf("\n EPDM data o %d sent success\n ******//sleeping *******\n ", j + 1);
        }
        //sleep(1);
      }

    }

    /*
    Command for Enabling status of KILL SWITCH
    */
    else if (cmds[0] == 0xee && cmds[1] == 0xaa && cmds[2] == 0xaa)
    {
      if (HAL_UART_Transmit(&COM_uart, ack, sizeof(ack), 1000 ) == HAL_OK){myprintf("Acknowledge sent successfully\n");}

      myprintf("---------Enable kill switch\n");
    }

    /*
    Command for Disabling status of KILL SWITCH
    */
    else if (cmds[0] == 0xee && cmds[1] == 0xee && cmds[2] == 0xee)
    {
      if (HAL_UART_Transmit(&COM_uart, ack, sizeof(ack), 1000 ) == HAL_OK){myprintf("Acknowledge sent successfully\n");}

      myprintf("---------Disable  kill switch\n");
    }
  }
  /* code */
  break;

  case COM_MCU:
    // Command to ENABLE Digipeater misison
    {
      myprintf("COM MCU ID has been received\n");
      if (cmds[0] == 0xDF && cmds[1] == 0xAB && cmds[2] == 0xD1)
      {

        myprintf("--------Disable  digipeater mission\n");
      }

      // Command to DISABLE Digipeater misison
      if (cmds[0] == 0xFD && cmds[1] == 0xBA && cmds[2] == 0xD0)
      {
        myprintf("-----------------------Enable  digipeater mission\n");
      }
    }
    /* code */
    break;

  case ADCS_MCU:
    // Command to DISABLE adcs(MSN1) misison
    {
      myprintf("ADCS MCU ID has been received\n");
      if (HAL_UART_Transmit(&COM_uart, ack, sizeof(ack), 1000 ) == HAL_OK){myprintf("Acknowledge sent successfully\n");}

      if (cmds[0] == 0xFD && cmds[1] == 0xBA && cmds[2] == 0xD0)
      {
        myprintf("------------ENable  adcs mission\n");
      }
    }
    /* code */
    break;

  case CAM_MCU:
    // Command to ENABLE/DISABLE or run camera(MSN2) mission
    {
      myprintf("CAM MCU ID has been received\n");
      if (cmds[0] == 0xCC && cmds[1] == 0x5E && cmds[2] == 0xBD)
      {
        myprintf("------------------------Enable  cam mission\n");
      }
    }
    /* code */
    break;

  case EPDM_MCU:
    // Command to ENABLE/DISABLE or run epdm(MSN3) mission
    {
      myprintf("EPDM MCU ID has been received\n");
      if (cmds[0] == 0xAC && cmds[1] == 0xCF && cmds[2] == 0xCF)
      {
        if (HAL_UART_Transmit(&COM_uart, ack, sizeof(ack), 1000 ) == HAL_OK){myprintf("Acknowledge sent successfully\n");}

        myprintf("----------------EPDM MCU ID has been activated\n");
      }
    }
    /* code */
    break;

  default:
    ack[1] = 0xac;
    ack[2] = 0x04;
    ack[4] = 0x63;
    ack[5] = 0x62;
    if (HAL_UART_Transmit(&COM_uart, ack, sizeof(ack), 1000 ) == HAL_OK){myprintf("Acknowledge sent successfully\n");}
    myprintf("The supplied command is incorrect");
    return;
    break;
  }

  // for (int i = 0; i < BEACON_DATA_SIZE; i++)
  // {
  //     send_data_uart(COM_UART, ack[i], 1);
  //     myprintf("%02x ",ack[i]);
  // }
}

/****************************************************************************
 * COM RX telecommands
 *
 * COM works this way:
 * telecommand receive
 * ack send
 * execute command
 *
 * Useful commands (12 bytes)
 *  0:      MCU ID
 *  1-3:    main command
 *  4-5:    reservation table commands
 *  6-9:    address data (if data is being downloaded)
 *  10-11:  no of packets (if data is being downloaded)
 ****************************************************************************/
int receive_telecommand_rx(uint8_t *COM_RX_DATA, lfs_t Lfs1, lfs_file_t File1)
{ // TODO sth to do with parsing
  uint8_t useful_command[12];
  uint8_t main_cmd[3] = {'\0'};
  uint16_t rsv_table = 0;
  uint32_t address = 0;
  uint16_t pckt_no = 0;
  uint8_t HEADER, MCU_ID;

  uint8_t ack[85] = {0x53, 0xac, 0x04, 0x01, 0x62, 0x63, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x70, 0x71, 0x72, 0x7e, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x80, 0x7e};
  myprintf("waiting for telecommands from COM\n");
  HAL_StatusTypeDef ret;
  // ret = 1;
  // TODO remove the comment line below and comment the upper line to int ret
  ret = HAL_UART_Receive(&COM_uart, COM_RX_DATA, COM_RX_CMD_SIZE,1000);//receive_data_uart(COM_UART, COM_RX_DATA, COM_RX_CMD_SIZE); // telecommand receive
  myprintf("Received ret as %d and value :%s\n", ret, COM_RX_DATA);
  if (ret != HAL_OK)
  {
    myprintf("data not received from COM\n NACK sending\n");
//    send_data_uart(COM_UART, NACK, 7);
    if(HAL_UART_Transmit(&COM_uart, NACK, 7, 1000) == HAL_OK){
    	myprintf("NACK sent successfully. Data not received");
    }
    myprintf("data not received from COM\n NACK Sent\n");
    return ret;
  }
  else
  {
    parse_command(COM_RX_DATA, Lfs1, File1);
  }
  myprintf("Value of digipeating is %d %d\n", digipeating, COM_RX_DATA[HEADER]);

  myprintf("value of ret is %d\ndata received from COM\n sending ACK\n", ret);
  //sleep(3);

  return ret;
}
