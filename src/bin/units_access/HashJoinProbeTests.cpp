#include "access/HashJoinProbe.h"
#include "access/HashBuild.h"
#include "io/shortcuts.h"
#include "storage/TableEqualityTest.h"
#include "testing/test.h"

namespace hyrise {
namespace access {

class HashJoinProbeTests : public AccessTest {};

TEST_F(HashJoinProbeTests, DISABLED_basic_hash_join_probe_test) {
        const std::string header_left("A|B|C\nINTEGER|STRING|FLOAT\n0_R|0_R|0_R");
        const std::string header_right("D|E|F\nINTEGER|STRING|FLOAT\n0_R|0_R|0_R");
        const std::string header_ref("A|B|C|D|E|F\nINTEGER|STRING|FLOAT|INTEGER|STRING|FLOAT\n0_R|0_R|0_R|0_R|0_R|0_R");
        auto left      = Loader::shortcuts::loadWithStringHeader("test/tables/hash_table_test.tbl", header_left);
        auto right     = Loader::shortcuts::loadWithStringHeader("test/tables/hash_table_test.tbl", header_right);
        auto reference = Loader::shortcuts::loadWithStringHeader("test/reference/hash_table_test_int.tbl", header_ref);

        HashBuild hb;
        hb.addInput(right);
        hb.addField(0);
        hb.setKey("join");
        hb.execute();

        auto right_hash = hb.getResultHashTable();

        HashJoinProbe hjp;
        hjp.addInput(left);
        hjp.addField(0);
        hjp.addInputHash(right_hash);
        hjp.execute();

        auto result = hjp.getResultTable();

        EXPECT_RELATION_EQ(result, reference);
}

}
}