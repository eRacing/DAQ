# Data Acquisition

## Message Format

- 0x0D (1 byte)
- Message number (4 bytes)
- Timestamp (4 bytes)
- CAN Message ID (4 byte)
- CAN Message Length (1 byte)
- CAN Message Data (up to 8 bytes)
- Padding with zero
