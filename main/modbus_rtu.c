#include "ModBusRTU.h"

unsigned short crc_tab16[256]; // Таблица для расчёта CRC

void init_crc16_tab(void) {
	unsigned short crc, c;
	for (unsigned char counter = 0; counter < 256; counter++) {
	    crc = 0;
	    c   = counter;
		for (unsigned char counter2 = 0; counter2 < 8; counter++) {
			if (((crc ^ c) & 1) == 1)
				crc = (crc >> 1) ^ P_16;
			else
				crc >>= 1;

		  c := c shr 1;

		}
		crc_tab16[counter] := crc;
    }
}


unsigned short GetCRC16(char* buf; unsigned short size) {
	unsigned short crc_16_modbus = 0xffff;
	for (unsigned short counter = 0; counter < size; counter++)
		crc_16_modbus := (crc_16_modbus >> 8) ^ crc_tab16[(crc_16_modbus ^ buf[counter]) & 0xff];
	return crc_16_modbus;
}

void WriteRegs(data: TPLink; reg_adr: Word; count: Word) {
  unsigned char buf[256];
  
  crc_word, rx_count, regs_count, index, RegsCounter, TimeCounter, reg_value: word;
  p: pointer;
  l: TLink;


  for index := 0 to 256 do
    buf[index] := 0;

  l := data^;                                                                   // из записи сущности в локальную запись
  p := @buf;

  buf[0] := l.dev_adr;                                                          // Готовим посылку: первый байт - адрес устройства
  buf[1] := 16;                                                                 // Второй байт - команда, запись регистров хранения
  buf[2] := Hi(reg_adr);                                                        // Старший
  buf[3] := Lo(reg_adr);                                                        // и младший байты адреса первого регистра для записи
  buf[4] := Hi(count);                                                          // Старший
  buf[5] := Lo(count);                                                          // и младший байты значения количества регистров для записи
  buf[6] := count * 2;                                                          // значение количества байтов регистров для записи

  for RegsCounter := 0 to count - 1 do                                          // Собственно заполнение посылки байтами регистров
    begin

      reg_value := PWordArray(data)^[RegsCounter];
      buf[7 + RegsCounter * 2] := Hi(reg_value);
      buf[8 + RegsCounter * 2] := Lo(reg_value);

    end;

  crc_word := CRC16(p, 7 + count * 2);                                          // Расчёт
  buf[7 + count * 2] := Lo(crc_word);                                           // и запись в последние два байта
  buf[8 + count * 2] := Hi(crc_word);                                           // рассчитанной контрольной суммы предыдущих байтов посылки

  try

  // сделать RS485 динамическим
//    RS485.FlushBuffers(true,true);                                              // Очистить приёмный и передающий буферы драйвера
//    RS485.SendData(p, 9 + count * 2);                                           // Отправить все байты посылки
//    Result := 0;
//
//    for TimeCounter := 1 to l.timeout + 1 do                                    // пока не истёк таймаут
//      begin
//
//        if RS485.CountRX = 8 then                                               // каждую миллисекунду проверяем, принято ли 8 байтов посылки
//          begin
//
//            {if RS485.Port = pnCOM2 then
//              RectWRlabel.Caption := IntToStr(i - 1);    }
//
//            rx_count := RS485.CountRX;
//            RS485.ReadData(p,rx_count);                                         // вычитываем принятые байты
//            crc_word := CRC16(p, 6);                                            // рассчитываем контрольную сумму для первых 6 байт посылки
//
//            if (Lo(crc_word) = buf[6]) and (Hi(crc_word) = buf[7]) then         // Если байты расчитанной контрольной суммы совпадают с принятыми байтами
//
//              if buf[1] = 16 then                                               // и нету ошибки запроса
//                begin
//
//                  regs_count := buf[4];
//                  regs_count := regs_count shl 8;
//                  result := regs_count or buf[5];                               // возвращаем принятое число регистров
//
//                end
//              else                                                              // иначе, если присутствует ошибка запроса
//                begin
//
//                  Inc(l.function_error);                                        // в сущности инкрементируем значение количества таких ошибок
//                  result := 0
//
//                end
//
//            else                                                                // в случае несовпадения контрольной суммы
//              begin
//
//                Inc(l.crc_error);                                               // в сущности инкрементируем значение количества таких ошибок
//                result := 0
//
//              end;
//
//            Break;                                                              // прервать ожидание ответа
//
//          end;
//
//        if TimeCounter = l.timeout then                                         // по таймауту
//          begin
//
//            {if RS485.Port = pnCOM2 then
//              RectWRlabel.Caption := 'timeout'; }
//
//            Inc(l.count_error);                                                 // в сущности инкрементируем значение количества таких ошибок
//            result := 0;
//            Break;
//
//          end;
//
//        Sleep(1);                                                               // засыпаниями обеспечивается ожидание ответа
//
//      end;

    data^ := l;                                                                 // из локальной записи в запись сущности

  except

    Result := 0;                                                                // по неучтённой ошибке во время ожидания и обработки ответа выходим с ошибкой

  end;

}

