/*
 * create by why @ 2009 06 10 13:30
 * read & write ci io space
 */

unsigned int read_ci(unsigned int cardtype,unsigned char* buff, int buf_len)
{
    return read_card_io_2(cardtype,buff, buf_len);
}

