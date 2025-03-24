library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity taggen2 is
    port (s   : in  state2_t_arr;
          tag : out std_logic_vector(127 downto 0));
end entity;

architecture parallel of taggen2 is
begin

    tag <= s(0) xor s(1) xor s(2) xor s(3) xor s(4) xor s(5) xor s(6) xor s(7) xor s(8);

end architecture;

