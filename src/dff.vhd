library ieee;
use ieee.std_logic_1164.all;

entity dff is
    port (clk : in std_logic;

          en : in std_logic;
          d  : in std_logic;
          q  : out std_logic);
end entity;

architecture behavioural of dff is
begin

    flipflop : process(clk)
    begin
        if rising_edge(clk) then
            if en = '1' then
                q <= d;
            end if;
        end if;
    end process;

end architecture;
