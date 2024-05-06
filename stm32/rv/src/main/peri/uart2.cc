#include "user_config.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <peri/uart.h>
#include <cstring>
#include <cstdio>
#include "loop/loop.hh"

struct DisableIRQ {
  DisableIRQ() {
    USART_CR1(USART2) = USART_CR1(USART2) & ~USART_CR1_RXNEIE;
  }
  ~DisableIRQ() {
    USART_CR1(USART2) = USART_CR1(USART2) | USART_CR1_RXNEIE;
  }
};

template<unsigned BUF_SIZE_BITS = 8, typename T = volatile uint8_t>
class UART_Buffer {
  static_assert(sizeof(T)*8 >= BUF_SIZE_BITS);

  static constexpr unsigned RX_BUFSIZE = (1 << BUF_SIZE_BITS);
  T m_head = 0, m_tail = 0;
  uint8_t m_buf[RX_BUFSIZE];

private:
  inline void advance(T &ct) {
    ct = (ct + 1) & (RX_BUFSIZE - 1);
  }

public:
  bool isFull() const {
    return ((m_tail + 1) & (RX_BUFSIZE - 1)) == m_head;
  }
  bool isEmpty() const {
    return m_head == m_tail;
  }

  bool putc_unguarded(uint8_t c) {
    if (isFull())
      return false;

    m_buf[m_tail] = c;
    advance(m_tail);
    return true;
  }
  int getc_unguarded(void) {
    if (isEmpty())
      return -1;

    int result = m_buf[m_head] & 0xFF;
    advance(m_head);
    return result;
  }

public:

  bool putc(uint8_t c) {
    DisableIRQ dont_interrupt_me;
    return putc_unguarded(c);
  }

  int getc() {
    DisableIRQ dont_interrupt_me;
    return getc_unguarded();
  }

  unsigned read(char *buf, const unsigned buf_size) {
    DisableIRQ dont_interrupt_me;

    for (unsigned int i = 0; i < buf_size; ++i) {
      int c = getc_unguarded();
      if (c == -1)
        return i;
      buf[i] = c;
    }

    return buf_size;
  }
};

static UART_Buffer<7, volatile uint8_t> uart1_rx_buf;

int usart2_getc(void) {
  return uart1_rx_buf.getc();
}

int usart2_read(char *buf, size_t buf_size) {
  return uart1_rx_buf.read(buf, buf_size);
}

int usart2_write(const char *data, size_t data_len) {
  size_t i;
  for (i = 0; i < data_len; ++i)
    usart_send_blocking(USART2, data[i]);
  return i;
}

int usart2_puts(const char *s) {
  return usart2_write(s, std::strlen(s));
}

int usart2_putc(char c) {
  usart_send_blocking(USART2, c);
  return 1;
}


extern "C" void usart2_isr(void) {
  /* Check if we were called because of RXNE. */
  if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) && ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {

    /* Retrieve the data from the peripheral. */
    uint16_t data = usart_recv(USART2);

    if (!uart1_rx_buf.putc_unguarded(data & 0xff))
      lf_setBit(lf_uart2_rx_buffer_full);

    lf_setBit(lf_cli);
  }
}
#if 0
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE  int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE  int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
 usart_send_blocking(USART2, ch);
 return ch;
}
#endif


static void uart_setup_stdio() {
  setbuf(stdout, nullptr);
}

void uart2_setup(void) {
  /* Enable the USART2 interrupt. */
  nvic_enable_irq(NVIC_USART2_IRQ);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART2);
  gpio_set_mode(GPIO_BANK_USART2_TX, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
  gpio_set_mode(GPIO_BANK_USART2_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

  /* Setup UART parameters. */
  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  /* Enable USART2 Receive interrupt. */
  USART_CR1(USART2) = USART_CR1(USART2) | USART_CR1_RXNEIE;

  /* Finally enable the USART. */
  usart_enable(USART2);

  uart_setup_stdio();
}
