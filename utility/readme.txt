compressed file specification:
[0..1] - size of frequency dictionary (fds) *
[1..10], [10..19] .. [..fds+1] - dictionary description in format: 1b - key *, 8b - value *
<1b> - bit_size of expected_offset (in the end of file) *
...
[data]
expected_offset

* - normalized [-128, 128)
