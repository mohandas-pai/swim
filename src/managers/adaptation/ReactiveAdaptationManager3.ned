//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package plasa.managers.adaptation;
//
// TODO auto-generated module
//
simple ReactiveAdaptationManager3 like IAdaptationManager
{
    parameters:
        bool simulateDecisionDelay = default(true);

        @signal[decisionTime](type="double");
        @statistic[decisionTime](unit=msec; record=mean,max,vector);

        @class(ReactiveAdaptationManager2);
}
