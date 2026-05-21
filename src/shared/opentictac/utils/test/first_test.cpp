#include "catch.hpp"
#include "../pwl_line.hpp"

TEST_CASE("test add", "[add]")
{

  PwlLines l1(0., 1., 1., 2.);
  PwlLines l2(0.5, 1, 1.5, 2);
  PwlLines l3(3.5, 1, 4.5, 2);
  PwlLines l4(3.5, 1, 4.5, 2);
  PwlLines l5(3.5, 1, 4.5, 2);
  PwlLines l6(4.5, 2, 5.5, 1);
  PwlLines l7(3.5, 1, 4.5, 2);
  PwlLines l8(0.5, 0., 1.5, 0.);
  PwlLines l9(1.00001, 2, 1.5, 3.);
  PwlLines l10(-1, -1, 1, -3);
  PwlLines l11(-0.5, -1, 1.5, -3);
  PwlLines empty;

  SECTION("Check connected lines")
  {
    CHECK(
        l1 + l2 ==
        PwlLines(std::vector<valPair>{
            {0., 0.}, {0., 1.}, {0.5, 1.5}, {0.5, 2.5}, {1, 3.5}, {1, 1.5}, {1.5, 2}, {1.5, 0.}}));


    CHECK(l1 + l3 ==
          PwlLines(
              {{0., 0}, {0., 1.}, {1., 2.}, {1., 0.}, {3.5, 0.}, {3.5, 1.}, {4.5, 2.}, {4.5, 0.}}));
    CHECK(l3 + l4 ==
          PwlLines(
              { {3.5, 0.}, {3.5, 2.}, {4.5, 4.}, {4.5, 0.}}));

    CHECK(l5 + l6 == PwlLines({{3.5, 0.}, {3.5, 1.}, {4.5, 2.}, {5.5, 1.}, {5.5, 0.}}));

    CHECK(l1 + l8 == PwlLines({{0, 0}, {0, 1}, {0.5, 1.5}, {1, 2}, {1, 0}, {1.5, 0}}));
    CHECK(l2 + l8 == PwlLines({{0.5, 0.}, {0.5, 1}, {1.5, 2}, {1.5, 0}}));
    CHECK(
        l1 + l9 ==
        PwlLines({{0, 0}, {0, 1}, {1, 2}, {1, 0}, {1.00001, 0}, {1.00001, 2}, {1.5, 3}, {1.5, 0}}));

    CHECK(l2 + l1 == l1 + l2);
    CHECK(l3 + l1 == l1 + l3);
    CHECK(l3 + l4 == l4 + l3);
    CHECK(l5 + l6 == l6 + l5);
    CHECK(l1 + l8 == l8 + l1);
    CHECK(l2 + l8 == l8 + l2);
    CHECK(l1 + l9 == l9 + l1);


    CHECK(l10 + l11 ==
          PwlLines({{-1, -1}, {-0.5, -1.5}, {-0.5, -2.5}, {1, -5.5}, {1, -2.5}, {1.5, -3}}));

    // test of constructors
    CHECK(l1 == PwlLines({{0, 0}, {0, 1}, {1, 2}, {1, 0}}));
    CHECK(l1 == PwlLines({{0, 1}, {1, 2}}));
    CHECK(l1 == PwlLines({{0, 0}, {0, 1}, {1, 2}}));
    // test pruning
    CHECK(l1 == PwlLines({{0, 0}, {0,1},{0, 1}, {1, 2}, {1, 0}}));
    CHECK(l1 == PwlLines({{0, 0}, {0, 0.5}, {0, 1}, {1, 2}, {1, 0}}));
  }

  SECTION("Check simple constructor")
  {
    CHECK(l1 + empty == l1);
    CHECK(empty + l1 == l1);
  }
}


TEST_CASE("test eval", "[eval]")
{
  // PwlLines (0.,1.,1,2.);
  std::vector<PwlLines> lines{PwlLines({{0., 1.}, {1., 2.}}), PwlLines({{0, 1}, {1, 2}, {2, 0}})};


  CHECK(lines[0].eval(-1.0) == 0.0);
  CHECK(lines[0].eval(0.0) == 1.0);
  CHECK(lines[0].eval(0.5) == 1.5);
  CHECK(lines[0].eval(1.0) == 2.0);
  CHECK(lines[0].eval(1.5) == 0.0);

  CHECK(lines[1].eval(-1.0) == 0.0);
  CHECK(lines[1].eval(0.0) == 1.0);
  CHECK(lines[1].eval(0.5) == 1.5);
  CHECK(lines[1].eval(1.0) == 2.0);
  CHECK(lines[1].eval(1.5) == 1.0);
  CHECK(lines[1].eval(2.0) == 0.0);
}


TEST_CASE("test mult", "[mult]")
{

  std::vector<PwlLines> lines{PwlLines({{0., 1.}, {1., 2.}}),
                              PwlLines({{0, 1}, {1, 2}, {2, 0}}),
                              PwlLines({{-1, 1}, {1, -1}}),
                              PwlLines({{1, 0.5}, {2, 0.75}}),
                              PwlLines({{0, 2}, {0.5, 3}})};

  CHECK(lines[0].mult(0, 1, 2) == PwlLines({{0, 2}, {1, 4}}));
  CHECK(lines[0].mult(0, 1, 4) == PwlLines({{0, 4}, {1, 8}}));
  CHECK(lines[0].mult(0, 1, -2) == PwlLines({{0, -2}, {1, -4}}));
  CHECK(lines[0].mult(0, 1, 0) == PwlLines({{0, 0}, {1, 0}}));

  CHECK(lines[2].mult(-1, 1, -2) == PwlLines({{-1, -2}, {1, 2}}));
  CHECK(lines[2].mult(-1, 0, -2) == PwlLines({{-1, -2}, {0, 0}, {1, -1}}));

  CHECK(lines[0].mult(0.5, 1.5, 2) == PwlLines({{0, 1}, {0.5, 1.5}, {0.5, 3}, {1.0, 4}}));
  CHECK(lines[0].mult(-0.5, 0.5, 4) == PwlLines({{0, 4}, {0.5, 6}, {0.5, 1.5}, {1.0, 2}}));
  CHECK(lines[0].mult(-0.5, 0.5, 0) == PwlLines({{0, 0}, {0.5, 0}, {0.5, 1.5}, {1.0, 2.0}}));

  CAPTURE(lines[0] + lines[3]);
  CAPTURE((lines[0] + lines[3]).innerSection(1, 2));
  CHECK((lines[0] + lines[3]).mult(1, 2, 4) == PwlLines({{0, 1}, {1, 2}, {2, 3}}));
  CHECK((lines[0] + lines[3]).mult(0.5, 2, 4) ==
        PwlLines({{0, 1}, {0.5, 1.5}, {0.5, 6}, {1, 8}, {1, 2}, {2, 3}}));
  CHECK((lines[0] + lines[3]).mult(1.5, 2, 4) ==
        PwlLines({{0, 1}, {1, 2}, {1.0, 0.5}, {1.5, 0.625}, {1.5, 2.5}, {2, 3}}));

  CHECK(lines[4].mult(0, 1, 1) == PwlLines({{0, 2}, {0.5, 3}}));
}


TEST_CASE("inner section", "[section]")
{
  std::vector<PwlLines> lines{PwlLines({{0., 1.}, {1., 2.}}),
                              PwlLines({{0, 1}, {1, 2}, {2, 0}}),
                              PwlLines({{-1, 1}, {1, -1}})};
  CAPTURE(lines[0]);
  CHECK(lines[0].innerSection(0.5, 1.5) == PwlLines({{0.5, 1.5}, {1.0, 2.}, {1.0, 0.0}}));
  CHECK(lines[0].innerSection(-0.5, 1.5) == PwlLines({{0., 1.}, {1.0, 2.}}));
  CHECK(lines[0].innerSection(-0.5, 0.5) == PwlLines({{0., 1.}, {0.5, 1.5}}));
  CAPTURE(lines[1]);
  CHECK(lines[1].innerSection(0.5, 1.5) == PwlLines({{0.5, 1.5}, {1.0, 2.0}, {1.5, 1.}}));
}


TEST_CASE("test pwl output", "[pwl]")
{

  std::vector<PwlLines> lines{PwlLines({{0., 1.}, {1., 2.}}),
                              PwlLines({{0, 1}, {1, 2}, {2, 0}}),
                              PwlLines({{-1, 1}, {1, -1}})};
  CAPTURE(lines[0]);
  CHECK(lines[0].getPwlValues(1e3) ==
        std::vector<MYREAL>({0.0, 0.0, 0.001, 1.0, 1.0, 2.0, 1.002, 0.0}));
  CHECK(lines[0].getPwlValues(1e12) ==
        std::vector<MYREAL>({0.0, 0.0, 1e-12, 1.0, 1.0, 2.0, 1.+2e-12, 0.0}));
  CHECK(
      lines[0].getPwlValues() ==
      std::vector<MYREAL>(
          {0.0, 0.0, 1e-5, 1.0, 1.0, 2.0, 1. + 2e-5, 0.0}));
}

TEST_CASE("test constant detection", "[const]")
{

  std::vector<PwlLines> lines{
      PwlLines({{0., 1.}, {1., 2.}}),
      PwlLines({{0, 1}, {1, 1}, {1, 0}}),
      PwlLines({{-1, 1}, {1, -1}}),
      PwlLines({{0, 1}, {1, 1}, {1, 2}, {2, 2}}),
  };

  SECTION("detection")
  {
    CHECK(!lines[0].isConstant());
    CHECK(lines[1].isConstant());
    CHECK(!lines[2].isConstant());
    CHECK(lines[3].isConstant());
  }

  SECTION("extraction")
  {
    CHECK(lines[1].getConstantParts() ==
          (std::vector<std::tuple<MYREAL, MYREAL, MYREAL>>({{0, 1, 1}})));
    CHECK(lines[3].getConstantParts() ==
          (std::vector<std::tuple<MYREAL, MYREAL, MYREAL>>({{0, 1, 1}, {1, 2, 2}})));
  }
}

