package main

import (
	"fmt"
	"os"
	"strconv"
)

// 级数	累计预扣预缴应纳税所得额		预扣率（%）	速算扣除数
// 1	不超过36000元的部分				3			0
// 2	超过36000元至144000元的部分		10			2520
// 3	超过144000元至300000元的部分	20			16920
// 4	超过300000元至420000元的部分	25			31920
// 5	超过420000元至660000元的部分	30			52920
// 6	超过660000元至960000元的部分	35			85920
// 7	超过960000元的部分				45			181920

// A同学2019年每月应发工资均为30000元，每月减除费用5000元，“三险一金”等专项扣除为4500元，享受子女教育、赡养老人两项专项附加扣除共计2000元
// 以前三个月为例，应当按照以下方法计算各月应预扣预缴税额：
//       1月份：（30000–5000-4500-2000）×3% = 555元；
//       2月份：（30000×2-5000×2-4500×2-2000×2）×10% -2520 -555 =625元；
//       3月份：（30000×3-5000×3-4500×3-2000×3）×10% -2520 -555-625 =1850元；
// 上述计算结果表明，由于2月份累计预扣预缴应纳税所得额为37000元，已适用10%的税率，因此2月份和3月份应预扣预缴有所增高。

func main() {
	if len(os.Args) != 4 {
		fmt.Println("使用方法：输入 3 个参数并以空格分开，分别是 税前收入 五险一金 专项扣除额度")
		fmt.Println("如 ./tax 30000 4500 2000")
		return
	}

	const EXEMPTION int = 5000 // 免征额

	var personalIncome int   // 税前收入
	var socialSecurity int   // 五险一金
	var specialDeduction int // 专项扣除

	personalIncome, _ = strconv.Atoi(os.Args[1])
	socialSecurity, _ = strconv.Atoi(os.Args[2])
	specialDeduction, _ = strconv.Atoi(os.Args[3])

	fmt.Println("--------------------------------------------------------")
	fmt.Printf("税前收入：%d, 五险一金：%d, 专项扣除：%d\n", personalIncome, socialSecurity, specialDeduction)
	fmt.Println("--------------------------------------------------------")

	var curMonthTax, totalTax float32 // 上一个月预扣

	for i := 1; i <= 12; i++ {
		curMonthTax = float32((personalIncome - EXEMPTION - socialSecurity - specialDeduction) * i)

		if curMonthTax < 36000 {
			curMonthTax = curMonthTax*0.03 - 0 - totalTax
		} else if curMonthTax < 144000 {
			curMonthTax = curMonthTax*0.10 - 2520 - totalTax
		} else if curMonthTax < 300000 {
			curMonthTax = curMonthTax*0.20 - 16920 - totalTax
		} else if curMonthTax < 420000 {
			curMonthTax = curMonthTax*0.25 - 31920 - totalTax
		} else if curMonthTax < 660000 {
			curMonthTax = curMonthTax*0.30 - 52920 - totalTax
		} else if curMonthTax < 960000 {
			curMonthTax = curMonthTax*0.35 - 85920 - totalTax
		} else {
			curMonthTax = curMonthTax*0.45 - 181920 - totalTax
		}

		fmt.Printf("%2d 月缴税 %.2f\n", i, curMonthTax)

		totalTax += curMonthTax
	}

	fmt.Println("--------------------------------------------------------")
	fmt.Printf("共缴税    %.2f\n", totalTax)
	fmt.Println("--------------------------------------------------------")
}
