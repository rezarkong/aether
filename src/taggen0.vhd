library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity taggen0 is
    port (s   : in  state0_t_arr;
          tag : out std_logic_vector(127 downto 0));
end entity;

architecture parallel of taggen0 is
begin

    tag <= s(0) xor s(1) xor s(2) xor s(3);

end architecture;

