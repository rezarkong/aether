library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;

entity state_reg is
    generic (size : integer := 128);
    port (clk : in std_logic;
          
          d   : in std_logic_vector(size-1 downto 0);
          q : out std_logic_vector(size-1 downto 0));
end entity;

architecture structural of state_reg is

	constant en : std_logic := '1';
begin

    bank : for i in 0 to size-1 generate
        dff_gen : entity dff port map (clk, en, d(i), q(i));
    end generate;

end architecture;

