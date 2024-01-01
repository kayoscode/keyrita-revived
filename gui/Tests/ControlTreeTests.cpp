#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "XmlToUi.h"
#include "StandardControls.h"
#include "ControlAccessUtils.h"

using namespace wgui;

namespace
{
	std::string TestLayout = 
		R"(
<GuiRoot>
  <MenuBar>
    <StaticGrid Height="24" Width1="35"
                            Width2="35"
                            Width3="75"
                            Width4="70">
      <Menu Text="File" Width="150" Height="30" Tag="Tag1">
        <DynamicRow Height="40">
          <MenuItem Text="New"/>
        </DynamicRow>
      </Menu>
      <Menu Text="Edit" Width="200" Height="250" Tag="Tag1">
        <DynamicRow Height="40">
          <MenuItem Text="Undo"/>
        </DynamicRow>
        <DynamicRow Height="40">
          <MenuItem Text="Redo"/>
        </DynamicRow>
      </Menu>
    </StaticGrid>
  </MenuBar>

  <Group Scrollable="true">
    <DynamicRow Height="30" Tag="Tag1">
      <Checkbox Text="Checkbox1" Checked="True"/>
      <Checkbox Text="Checkbox2" Checked="False"/>
      <Checkbox Text="Checkbox3" Checked="True"/>
    </DynamicRow>
    <RadioButtonGroup Selection="2" Tag="RadioButtonGroup">
      <DynamicRow Height="30">
        <RadioButton Text="RB 1"/>
        <RadioButton Text="RB 2"/>
      </DynamicRow>
    </RadioButtonGroup>
</GuiRoot>
		)";

	std::vector<std::unique_ptr<GuiControlBase>> OwnedControls;
	std::vector<GuiControlBase*> RootControls;

   constexpr int ControlCount = 19;

	void CreateTestControls()
	{
      ASSERT_TRUE(XmlToUiUtil::ConstructLayoutFromXmlText(TestLayout, OwnedControls, RootControls));
	}
}

TEST(ControlTests, ControlFactoryTest)
{
   ASSERT_EQ(OwnedControls.size(), ControlCount);

   // Ensure correct attributes.
   GuiLayoutRowStaticGrid* staticGridCtrl = nullptr;
   GuiMenu* fileMenu = nullptr;

   for (int i = 0; i < OwnedControls.size(); i++)
   {
      if (dynamic_cast<GuiLayoutRowStaticGrid*>(OwnedControls[i].get()))
      {
         staticGridCtrl = reinterpret_cast<GuiLayoutRowStaticGrid*>(OwnedControls[i].get());
      }
      else if (dynamic_cast<GuiMenu*>(OwnedControls[i].get()))
      {
         if (OwnedControls[i]->GetAttributes()->Get<AttrString>("Text")->Get() == "File")
         {
            fileMenu = reinterpret_cast<GuiMenu*>(OwnedControls[i].get());
         }
      }
   }

   ASSERT_NE(staticGridCtrl, nullptr);
   ASSERT_NE(fileMenu, nullptr);

   // It should have four width attributes (attributes are case insensitive, test that here too)
   ASSERT_TRUE(staticGridCtrl->AttributeExists("WIDTH1"));
   ASSERT_TRUE(staticGridCtrl->AttributeExists("width2"));
   ASSERT_TRUE(staticGridCtrl->AttributeExists("Width3"));
   ASSERT_TRUE(staticGridCtrl->AttributeExists("Width4"));
   ASSERT_FALSE(staticGridCtrl->AttributeExists("Width5"));

   // The file menu should have a width, height and a tag.
   ASSERT_TRUE(fileMenu->AttributeExists("Width"));
   ASSERT_TRUE(fileMenu->AttributeExists("Height"));
   ASSERT_TRUE(fileMenu->AttributeExists("Tag"));
   ASSERT_EQ(fileMenu->GetTag(), "Tag1");
   ASSERT_EQ(fileMenu->GetTag(), fileMenu->GetAttributes()->Get<AttrString>("tag")->Get());
}

/// <summary>
/// Verify correct functionality of the control iterator.
/// Order doesn't matter for this iterator.
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(ControlTests, ControlTreeIteratorTests)
{
   std::map<GuiControlBase*, bool> visited;

   for (int i = 0; i < OwnedControls.size(); i++)
   {
      visited[OwnedControls[i].get()] = false;
   }

   // Ensure the iterator goes over all owned controls.
   for (int i = 0; i < RootControls.size(); i++)
   {
      for (auto it = RootControls[i]->begin(); it != RootControls[i]->end(); ++it)
      {
         ASSERT_FALSE(visited[*it]);
         visited[*it] = true;
      }
   }

   for (auto it : visited)
   {
      ASSERT_TRUE(it.second);
   }
}

TEST(ControlTests, GetControlsWithTagTests)
{
   // Test with the owned controls vector and with the tree iterator.
   std::vector<GuiMenuBar*> foundControls;
   ControlAccessUtils::GetControlsWithType(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()), foundControls);

   ASSERT_EQ(foundControls.size(), 1);
   ASSERT_NE(foundControls[0], nullptr);
   foundControls.clear();

   ControlAccessUtils::GetControlsWithType<GuiMenuBar, ControlTreeIterator>
      (RootControls[0]->begin(), RootControls[0]->end(), foundControls);

   ASSERT_EQ(foundControls.size(), 1);
   ASSERT_NE(foundControls[0], nullptr);

   std::vector<GuiMenuBar*> newFoundControls;
   ControlAccessUtils::GetControlsWithType<GuiMenuBar, std::vector<GuiMenuBar*>::iterator>
      (foundControls.begin(), foundControls.end(), newFoundControls);

   ASSERT_EQ(newFoundControls.size(), 1);
   ASSERT_EQ(foundControls[0], newFoundControls[0]);
   foundControls.clear();

   // Test getting controls with a specified tag.
   std::vector<GuiControlBase*> taggedControls;
   ControlAccessUtils::GetControlsWithTag(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()), "Tag1", taggedControls);
   ASSERT_EQ(taggedControls.size(), 3);
   for (int i = 0; i < taggedControls.size(); i++)
   { 
      ASSERT_NE(taggedControls[i], nullptr);
   }

   // Get control with attribute tests of type.
   std::vector<GuiMenu*> menuControls;
   ControlAccessUtils::GetControlsWithTag(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()), "Tag1", menuControls);
   ASSERT_EQ(menuControls.size(), 2);
   for (int i = 0; i < menuControls.size(); i++)
   {
      ASSERT_NE(menuControls[i], nullptr);
   }

   GuiControlBase* menuBar = ControlAccessUtils::GetUniqueControlWithType(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()));
   ASSERT_NE(menuBar, nullptr);

   GuiRadioButtonGroup* rbGroup = ControlAccessUtils::GetUniqueControlWithTag<GuiRadioButtonGroup>(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()), 
      "RadioButtonGroup");
   ASSERT_NE(rbGroup, nullptr);
}

TEST(ControlTests, GetControlsWithAttributeTests)
{
   std::vector<GuiControlBase*> foundControls;
   ControlAccessUtils::GetControlsWithAttribute(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()), 
      "Height",
      foundControls);

   ASSERT_EQ(foundControls.size(), 8);
   for (int i = 0; i < foundControls.size(); i++)
   {
      ASSERT_NE(foundControls[i], nullptr);
   }
   
   foundControls.clear();
   for (int i = 0; i < RootControls.size(); i++)
   {
      ControlAccessUtils::GetControlsWithAttribute(
         RootControls[i]->begin(),
         RootControls[i]->end(),
         "Height",
         foundControls);
   }

   ASSERT_EQ(foundControls.size(), 8);
   for (int i = 0; i < foundControls.size(); i++)
   {
      ASSERT_NE(foundControls[i], nullptr);
   }

   foundControls.clear();

   // Get controls with attribute and value tests.
   ControlAccessUtils::GetControlsWithAttributeValue<AttrInt, int64_t>(
      OwnedControlsIterator(OwnedControls.begin()),
      OwnedControlsIterator(OwnedControls.end()),
      "Height",
      30,
      foundControls);

   // Should find two dynamic rows and a menu.
   ASSERT_EQ(foundControls.size(), 3);
   ASSERT_NE(dynamic_cast<GuiMenu*>(foundControls[0]), nullptr);
   ASSERT_NE(dynamic_cast<GuiLayoutRowDynamic*>(foundControls[1]), nullptr);
   ASSERT_NE(dynamic_cast<GuiLayoutRowDynamic*>(foundControls[2]), nullptr);

   // Find controls with value based on type tests.
   std::vector<GuiLayoutRowDynamic*> foundRows;
   ControlAccessUtils::GetControlsWithAttributeValue<AttrInt, int64_t>(
      foundControls.begin(), foundControls.end(),
      "Height", 30,
      foundRows
   );

   std::vector<GuiMenu*> foundMenus;
   ControlAccessUtils::GetControlsWithAttributeValue<AttrString, std::string>(
      foundControls.begin(), foundControls.end(),
      "Text", "File",
      foundMenus
   );

   ASSERT_EQ(foundMenus.size(), 1);
   ASSERT_NE(foundMenus[0], nullptr);

   GuiMenu* width150 = ControlAccessUtils::GetUniqueControlWithAttributeValue<AttrInt, int64_t, GuiMenu>(
      OwnedControlsIterator(OwnedControls.begin()), OwnedControlsIterator(OwnedControls.end()),
      "Width", 150
   );
   ASSERT_NE(width150, nullptr);

   GuiControlBase* newAttr = ControlAccessUtils::GetUniqueControlWithAttribute(
      OwnedControlsIterator(OwnedControls.begin()), OwnedControlsIterator(OwnedControls.end()),
      "Width1"
   );
   ASSERT_NE(newAttr, nullptr);

   newAttr = ControlAccessUtils::GetUniqueControlWithAttribute(
      OwnedControlsIterator(OwnedControls.begin()), OwnedControlsIterator(OwnedControls.end()),
      "Width111"
   );
   ASSERT_EQ(newAttr, nullptr);
}

int main(int argc, char** argv)
{
	// Initialize the control factory with the standard control list.
	XmlToUiUtil::Init();
	CreateTestControls();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}