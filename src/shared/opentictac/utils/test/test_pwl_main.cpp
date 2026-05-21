#include <iostream>
#include "../pwl_line.hpp"

int
    main()
{
  PwlLines l1(0., 1., 1., 2.);
  PwlLines l2(0.5, 1, 1.5, 2);
  PwlLines l3(3.5, 1, 4.5, 2);
  PwlLines l4(3.5, 1, 4.5, 2);
  PwlLines l5(3.5, 1, 4.5, 2);
  PwlLines l6(4.5, 2, 5.5, 1);
  PwlLines l7(3.5, 1, 4.5, 2);
  PwlLines l8(0.5, 0., 1.5, 0.);

  std::cout << l5 << std::endl;
  std::cout << l6 << std::endl;
  std::vector<PwlLines> erg;
  // erg.push_back(l1.add(l2));
  // erg.push_back(l2.add(l1));
  // erg.push_back(l3.add(l1));
  // erg.push_back(l1.add(l3));
  // erg.push_back(l1 + l2);
  // erg.push_back(l2 + l1);
  // erg.push_back(l3 + l1);
  // erg.push_back(l1 + l3);
  // erg.push_back(l3 + l4);
  // erg.push_back(l5 + l6);
  erg.push_back(l6 + l5);
  // erg.push_back(l1 + l8);
  // erg.push_back(l8 + l1);
  for (auto p : erg)
    {
      /*
       * for (auto q : p)
       *   {
       *     std::cout << '|' << q << std::endl;
       *   }
       * std::cout << std::endl;
       */
      std::cout << p << std::endl;
    }
}

